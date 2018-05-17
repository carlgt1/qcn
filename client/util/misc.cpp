Metadata  mCountry, mCoastline;


void coordinate_struct_init(Vxp *vxp, char cType ) {
        int i, j, ii = -1;
        switch(cType) {
        case 'N': vxp->nFeatures = mCountry.elements;   break; // N = national boundaries
        case 'C': vxp->nFeatures = mCoastline.elements;            // C = continental coastlines
        }
        vxp->pFeatures = new VxpFeature[vxp->nFeatures];
        for ( i = 0 ; i < vxp->nFeatures; i++ ) {
                switch(cType) {
                case 'N': vxp->pFeatures[i].nVerts = mCountry.nodes[i];   break;
                case 'C': vxp->pFeatures[i].nVerts = mCoastline.nodes[i];
                }
                vxp->pFeatures[i].pVerts = new Vector3f[vxp->pFeatures[i].nVerts];
                for (j = 0; j < vxp->pFeatures[i].nVerts; j++) {
                        ii++;
                        switch(cType) {
                        case 'N' : if ((country[ii].lat == -1)  && (country[ii].lon == -1)) ii++;
                                           lonlat2point(country[ii].lon, country[ii].lat,&vxp->pFeatures[i].pVerts[j],EARTH_RADIUS);
                                           break;
                        case 'C' : if ((coastline[ii].x == -1) && (coastline[ii].y == -1))  ii++;
                                           vxp->pFeatures[i].pVerts[j].x  = coastline[ii].x * EARTH_RADIUS;
                                           vxp->pFeatures[i].pVerts[j].y  = coastline[ii].y * EARTH_RADIUS;
                                           vxp->pFeatures[i].pVerts[j].z  = coastline[ii].z * EARTH_RADIUS;
                        }
                }
        }
}


void graphics_components(){
        int i, x, y ;
        for ( i = 0; i < 2; i++ ){
                g_vxp[i] = new Vxp;
                memset(g_vxp[i], 0x00, sizeof(Vxp));
        }
        mCountry.elements = 0;
        for ( x = 0; x < NUM_NATION_BOUNDARIES; x++ )
                mCountry.nodes[mCountry.elements] = 0;
        mCoastline.elements = 0;
        for ( y = 0; y < NUM_COASTLINE; y++ )
                mCoastline.nodes[mCoastline.elements] = 0;
        for ( x = 0; x < NUM_NATION_BOUNDARIES; x++ ) {
                if ((country[x].lat == -1)&&(country[x].lon == -1)){
                        mCountry.elements++;
                        mCountry.nodes[mCountry.elements] = 0;
                }
                else mCountry.nodes[mCountry.elements]++;
        }
        for ( y = 0; y < NUM_COASTLINE; y++ ) {
                if ((coastline[y].x==-1)&&(coastline[y].y == -1)&&(coastline[y].z==-1)){
                        mCoastline.elements++;
                        mCoastline.nodes[mCoastline.elements] = 0;
                }
                else mCoastline.nodes[mCoastline.elements]++;
        }
        coordinate_struct_init(g_vxp[0], 'C'); // continental coastlines
        coordinate_struct_init(g_vxp[1], 'N'); // nation boundries
        pImage[0] = new Vector3uc[IMAGE_SCALE*CPDN_LONGITUDE*IMAGE_SCALE*CPDN_LATITUDE];
        pImage[1] = new Vector3uc[CPDN_LONGITUDE*CPDN_LATITUDE];
        memset(pImage[0], 0x00, sizeof(pImage[0]));
        memset(pImage[1], 0x00, sizeof(pImage[1]));
        memset(ucColor,   0x00, sizeof(ucColor));
        clear_image(pImage[1], CPDN_LONGITUDE,CPDN_LATITUDE, ucColor[0]);
}

void graphics_components(){
        int i, x, y ;
        for ( i = 0; i < 2; i++ ){
                g_vxp[i] = new Vxp;
                memset(g_vxp[i], 0x00, sizeof(Vxp));
        }
        mCountry.elements = 0;
        for ( x = 0; x < NUM_NATION_BOUNDARIES; x++ )
                mCountry.nodes[mCountry.elements] = 0;
        mCoastline.elements = 0;
        for ( y = 0; y < NUM_COASTLINE; y++ )
                mCoastline.nodes[mCoastline.elements] = 0;
        for ( x = 0; x < NUM_NATION_BOUNDARIES; x++ ) {
                if ((country[x].lat == -1)&&(country[x].lon == -1)){
                        mCountry.elements++;
                        mCountry.nodes[mCountry.elements] = 0;
                }
                else mCountry.nodes[mCountry.elements]++;
        }
        for ( y = 0; y < NUM_COASTLINE; y++ ) {
                if ((coastline[y].x==-1)&&(coastline[y].y == -1)&&(coastline[y].z==-1)){
                        mCoastline.elements++;
                        mCoastline.nodes[mCoastline.elements] = 0;
                }
                else mCoastline.nodes[mCoastline.elements]++;
        }
        coordinate_struct_init(g_vxp[0], 'C'); // continental coastlines
        coordinate_struct_init(g_vxp[1], 'N'); // nation boundries
        pImage[0] = new Vector3uc[IMAGE_SCALE*CPDN_LONGITUDE*IMAGE_SCALE*CPDN_LATITUDE];
        pImage[1] = new Vector3uc[CPDN_LONGITUDE*CPDN_LATITUDE];
        memset(pImage[0], 0x00, sizeof(pImage[0]));
        memset(pImage[1], 0x00, sizeof(pImage[1]));
        memset(ucColor,   0x00, sizeof(ucColor));
        clear_image(pImage[1], CPDN_LONGITUDE,CPDN_LATITUDE, ucColor[0]);
}

void render_3D_coastline() {
        glPushMatrix();
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glEnable(GL_DEPTH_TEST);
        render_coordinate(g_vxp[0]);     // continent coastlines
        if (nation_boundries)
                render_coordinate(g_vxp[1]); // nation boundries
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glPopMatrix();
}

void render_coordinate(Vxp *vxp) {
        int i, j;
        glLineWidth(lineWidth);
        if ( !use_default  && iCoastline > 0 && iCoastline <= NUM_RGB )
                glColor3d(rgb[iCoastline-1].r,rgb[iCoastline-1].g,rgb[iCoastline-1].b);
        else
                glColor3f(coastline_rgb[0], coastline_rgb[1], coastline_rgb[2]);
        for ( i = 0; i < vxp->nFeatures-1 ; i++ ) {
                glBegin(GL_LINE_STRIP);
                for ( j = 0; j < vxp->pFeatures[i].nVerts; j++ )
                        glVertex3fv((float*)&vxp->pFeatures[i].pVerts[j]);
                glEnd();
        }
}




