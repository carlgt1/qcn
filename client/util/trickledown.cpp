#include "main.h"
#include "trickledown.h"

#ifdef ZIPARCHIVE
#include "ZipArchive.h"
#else
#include "boinc_zip.h"
#endif

/*

CMC note: boinc trickledowns are used to request a host machine uploads files, or abort a workunit
note that the msg_to_host (trickledown database table) request must match the currently running result_name.
 
If an "old" result_name is sent it will be ignored (so it will be the current workunit running, even if you
are requesting an "old" file)

send file example:

insert into qcnalpha.msg_to_host
(create_time,hostid,variety,handled,xml)
values 
(unix_timestamp(),
26,
'filelist',0,
'<trickle_down>
<result_name>qcnb_000078_0</result_name>
<filelist>
<sendme>qcnb_000257_0003_1200263935.zip</sendme>
<sendme>qcnb_000279_0002_1200265119.zip</sendme>
</filelist>
</trickle_down>\n');

killer trickle example:

insert into qcnalpha.msg_to_host
(create_time,hostid,variety,handled,xml)
values 
(now(),
26,
'abort',0,
'<trickle_down>
<result_name>qcnb_000089_0</result_name>
<abort></abort>
</trickle_down>\n');

*/

namespace trickledown {

bool getTrickleDown(char* strTrickle, int iSize)
{
        bool bRet = false;
        char szTrickleFile[_MAX_PATH];
        memset(szTrickleFile, 0x00, _MAX_PATH);
        memset(strTrickle, 0x00, iSize);
        if (boinc_receive_trickle_down(szTrickleFile, _MAX_PATH)) {
          fprintf(stdout, "Got a trickle down from the QCN server!\n");

          // we just have a filename, need to read it in!
          FILE* fTrickle = boinc_fopen(szTrickleFile, "rb");
          if (fTrickle) {
             fseek(fTrickle, 0, SEEK_SET);  // rewind
             fread(strTrickle, 1, iSize, fTrickle);  // read at most 1KB
             fclose(fTrickle);
             // delete the trickle down file now that we're done with it
             boinc_delete_file(szTrickleFile);
             bRet = true;
          }
        }
        return bRet;
}

void processTrickleDown(void)
{
     char* szTrickleDown = new char[1024];
     memset(szTrickleDown, 0x00, 1024);
     if (!getTrickleDown(szTrickleDown, 1024)) {
        delete [] szTrickleDown;
        return;
     }

     // got a trickle down!

     if (strstr(szTrickleDown, TRICKLE_DOWN_FILELIST)) {
        fprintf(stdout, "Trickle down request for files to upload received...\n");

        //sending an xml list of files to upload 1...n
        char* q = szTrickleDown;
        char strResolve[_MAX_PATH], strZip[20];
        char strTemp[_MAX_PATH];
        char strFullPath[_MAX_PATH];
        ZipFileList zfl;  // build a zipfilelist

        memset(strResolve, 0x00, _MAX_PATH);
        memset(strZip, 0x00, 20);
        memset(strTemp, 0x00, _MAX_PATH);
        memset(strFullPath, 0x00, _MAX_PATH);

        // store the zip file # to resolve in the aiGMTrickle[1][1] I guess,
        // since I'm not using it for anything else!
        // get an empty zip slot to use --- 1 through 20 (MAX_UPLOAD)
        // we already have the slot from sm->iNumUpload
        int iSlot = (int) sm->iNumUpload; //+ pShMem->aiGMTrickle[1][1]++;  // this will get saved with shmem restart
        iSlot++;  // increment the upload slot counter
        if (iSlot<1 || iSlot>MAX_UPLOAD) {  // sanity check, only zip file #'s 1 through MAX_UPLOAD (20) reserved for intermediate uploading
          fprintf(stdout, "No zip slots left for file!\n");
          delete [] szTrickleDown;
          return;
        }

        // OK now we can resolve the zip filename
        // try and resolve the filename qcnout1.zip
        sprintf(strZip, "qcnout%d.zip", iSlot);
        if (boinc_resolve_filename(strZip, strResolve, _MAX_PATH) && !strResolve[0]) {
          // this zip name didn't resolve, free sz mem and return!
          fprintf(stdout, "Upload zip filename %s not resolved!\n", strZip);
          delete [] szTrickleDown;
          return;
        }

        while (q && parse_str((const char*) q, (const char*) TRICKLE_DOWN_SEND, strTemp, _MAX_PATH)) {
          strip_whitespace(strTemp);
          // note strPathTrigger DOES NOT HAVE / or \ on the end
          sprintf(strFullPath, "%s%c%s",
            qcn_main::g_strPathTrigger,
            qcn_util::cPathSeparator(),
            strTemp
          );
               
          if (boinc_file_exists(strFullPath))  {
            fprintf(stdout, "Trickle down request received to send file: %s\n", strFullPath);
            zfl.push_back(strFullPath);
          }
          q = strstr(q, TRICKLE_DOWN_SEND_END_TAG);
          if (q) q += (strlen(TRICKLE_DOWN_SEND_END_TAG) - 1);
        }

        if (zfl.size()) {  // there are valid entries so create the zip upload file & send
          sm->iNumUpload = iSlot;  // set the num upload which was successfully incremented & processed above
          sm->setTriggerLock();  // we can be confident we have locked the trigger bool when this returns
          qcn_util::set_qcn_counter();
          sm->releaseTriggerLock();

          // now do the zipping, remember files are in sm->strPathTrigger + filename requested
          if (!boinc_file_exists(strResolve)) { // don't already write an existing zip file!
            fprintf(stdout, "Zipping file: %s\n", strResolve);
            boinc_begin_critical_section();

#ifdef ZIPARCHIVE
       CZipArchive ziparch;
       bool bRetVal = false;
       try {

       bRetVal = ziparch.Open(strResolve, CZipArchive::zipCreate);
       if (!bRetVal) {
          fprintf(stdout, "Could not create zip archive %s!\n", strResolve);
       }

       for (unsigned int i = 0; bRetVal && i < zfl.size(); i++) {
             bRetVal = ziparch.AddNewFile(zfl[i].c_str(), -1, false);
       }
       ziparch.Close(CZipArchive::afWriteDir);

       }
       catch(...)
       { // throws exception on close error, but will write anyway
          bRetVal = false;
       }
       if (!bRetVal) {
         fprintf(stdout, "TrickleDown: CZip file error %d\n", 1);
         fflush(stdout);
       }
#else  // traditional boinc_zip
       boinc_zip(ZIP_IT, strResolve, &zfl);
#endif

            boinc_end_critical_section();
          }
          if (boinc_file_exists(strResolve)) { // send this file, whether it was just made or made previously
             // note boinc_upload_file (intermediate uploads) requires the logical boinc filename ("soft link")!
             qcn_util::sendIntermediateUpload(strZip, strResolve);  // the logical name gets resolved by boinc_upload_file into full path zip file 
          }
        }
     } // end trickle-down file sendlist

     // now check for abort message
     if (strstr(szTrickleDown, TRICKLE_DOWN_ABORT)) {
       // stop the press!
       fprintf(stdout, "Trickle down request received to abort this workunit\n");
       sm->eStatus = ERR_ABORT;  // set a flag the UM will pick up to abandon the run
     }

     // that's all, folks!
     delete [] szTrickleDown;

} // end of trickledown fn

}  // namespace trickledown

