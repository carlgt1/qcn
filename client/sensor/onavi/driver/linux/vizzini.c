/*
 * vizzini.c
 *
 * Copyright (c) 2010 Exar Corporation, Inc.
 *
 * ChangeLog:
 *            v0.1-2.4.18 - First version for 2.4.18 Kernel. Most of the code is
 *                   from 2.4.18 Kernel's usbserial.c, cdc.c, and Exar's earlier
 *                   version of Vizzini.c (credits due to Greg Kroah-Hartman
 *                   and Rob Duncan (of Exar)).
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/smp_lock.h>
#include <linux/usb.h>
#include <linux/serial.h>
#include <linux/ioctl.h>

#ifdef CONFIG_USB_SERIAL_DEBUG
	static int debug = 1;
#else
	static int debug;
#endif

#include "vizzini.h"

#ifndef CDC_DATA_INTERFACE_TYPE
#define CDC_DATA_INTERFACE_TYPE 0x0a
#endif
#ifndef USB_RT_ACM
#define USB_RT_ACM      (USB_TYPE_CLASS | USB_RECIP_INTERFACE)
#define ACM_CTRL_DTR            0x01
#define ACM_CTRL_RTS            0x02
#define ACM_CTRL_DCD            0x01
#define ACM_CTRL_DSR            0x02
#define ACM_CTRL_BRK            0x04
#define ACM_CTRL_RI             0x08
#define ACM_CTRL_FRAMING        0x10
#define ACM_CTRL_PARITY         0x20
#define ACM_CTRL_OVERRUN        0x40
#endif

#define ACM_REQ_SET_LINE	0x20
#define ACM_REQ_GET_LINE	0x21
#define ACM_REQ_SET_CONTROL	0x22
#define ACM_REQ_SEND_BREAK	0x23

#define USB_CDC_NOTIFY_NETWORK_CONNECTION	0x00
#define USB_CDC_NOTIFY_SERIAL_STATE		0x20

#define get_unaligned(ptr) (*(ptr))

/*
 * Version Information
 */
#define DRIVER_VERSION "v.0.1-2.4.18"
#define DRIVER_AUTHOR "Ravi Reddy <ravir@exar.com>"
#define DRIVER_DESC "USB Driver for Vizzini USB serial port"

/* local function prototypes */
static int  vizzini_open		(struct usb_serial_port *port, struct file *filp);
static void vizzini_close		(struct usb_serial_port *port, struct file *filp);
static int  vizzini_write		(struct usb_serial_port *port, int from_user, const unsigned char *buf, int count);
static int  vizzini_write_room		(struct usb_serial_port *port);
static int  vizzini_chars_in_buffer	(struct usb_serial_port *port);
static void vizzini_read_bulk_callback	(struct urb *urb);
static void vizzini_write_bulk_callback	(struct urb *urb);
static void vizzini_int_callback	(struct urb *urb);
static void vizzini_shutdown		(struct usb_serial *serial);
static int  serial_open (struct tty_struct *tty, struct file * filp);
static void serial_close (struct tty_struct *tty, struct file * filp);
static int  serial_write (struct tty_struct * tty, int from_user, const unsigned char *buf, int count);
static int  serial_write_room (struct tty_struct *tty);
static int  serial_chars_in_buffer (struct tty_struct *tty);
static void serial_throttle (struct tty_struct * tty);
static void serial_unthrottle (struct tty_struct * tty);
static int  serial_ioctl (struct tty_struct *tty, struct file * file, unsigned int cmd, unsigned long arg);
static void serial_set_termios (struct tty_struct *tty, struct termios * old);
static void serial_shutdown (struct usb_serial *serial);

static int			serial_refcount;
static struct tty_driver	serial_tty_driver;
static struct tty_struct *	serial_tty[SERIAL_TTY_MINORS];
static struct termios *		serial_termios[SERIAL_TTY_MINORS];
static struct termios *		serial_termios_locked[SERIAL_TTY_MINORS];
static struct usb_serial	*serial_table[SERIAL_TTY_MINORS];	/* initially all NULL */


static LIST_HEAD(usb_serial_driver_list);


static struct usb_serial *get_serial_by_minor (int minor)
{
	return serial_table[minor];
}


static struct usb_serial *get_free_serial (int num_ports, int *minor)
{
	struct usb_serial *serial = NULL;
	int i, j;
	int good_spot;

	dbg("%s %d", __FUNCTION__, num_ports);

	*minor = 0;
	for (i = 0; i < SERIAL_TTY_MINORS; ++i) {
		if (serial_table[i])
			continue;

		good_spot = 1;
		for (j = 1; j <= num_ports-1; ++j)
			if (serial_table[i+j])
				good_spot = 0;
		if (good_spot == 0)
			continue;

		if (!(serial = kmalloc(sizeof(struct usb_serial), GFP_KERNEL))) {
			err("%s - Out of memory", __FUNCTION__);
			return NULL;
		}
		memset(serial, 0, sizeof(struct usb_serial));
		serial->magic = USB_SERIAL_MAGIC;
		serial_table[i] = serial;
		*minor = i;
		dbg("%s - minor base = %d", __FUNCTION__, *minor);
		for (i = *minor+1; (i < (*minor + num_ports)) && (i < SERIAL_TTY_MINORS); ++i)
			serial_table[i] = serial;
		return serial;
	}
	return NULL;
}


static void return_serial (struct usb_serial *serial)
{
	int i;

	dbg("%s", __FUNCTION__);

	if (serial == NULL)
		return;

	for (i = 0; i < serial->num_ports; ++i) {
		serial_table[serial->minor + i] = NULL;
	}

	return;
}

static int acm_ctrl_msg(struct usb_serial_port *port,
                        int request, int value, void *buf, int len)
{
        struct usb_serial *serial = port->serial;
        int retval = usb_control_msg(serial->dev,
                                     usb_sndctrlpipe(serial->dev, 0),
                                     request,
                                     USB_RT_ACM,
                                     value,
                                     serial->interface->altsetting->bInterfaceNumber,
                                     buf,
                                     len,
                                     5000);
        dbg("acm_control_msg: rq: 0x%02x val: %#x len: %#x result: %d\n", request, value, len, retval);
        return retval < 0 ? retval : 0;
}


#define acm_set_control(port, control)                                  \
        acm_ctrl_msg(port, ACM_REQ_SET_CONTROL, control, NULL, 0)
#define acm_set_line(port, line)                                        \
        acm_ctrl_msg(port, ACM_REQ_SET_LINE, 0, line, sizeof *(line))
#define acm_send_break(port, ms)                                \
        acm_ctrl_msg(port, ACM_REQ_SEND_BREAK, ms, NULL, 0)



/*****************************************************************************
 * vizzini devices specific driver functions
 *****************************************************************************/

static int vizzini_set_reg(struct usb_serial *serial,
                           int block, int regnum, int value)
{
        int result;

        result = usb_control_msg(serial->dev,                     /* usb device */
                                 usb_sndctrlpipe(serial->dev, 0), /* endpoint pipe */
                                 XR_SET_REG,                      /* request */
                                 USB_DIR_OUT | USB_TYPE_VENDOR,   /* request_type */
                                 value,                           /* request value */
                                 regnum | (block << 8),           /* index */
                                 NULL,                            /* data */
                                 0,                               /* size */
                                 5000);                           /* timeout */

        return result;
}


static int vizzini_get_reg(struct usb_serial *serial,
                           int block, int reg, char *value)
{
        int result;

        result = usb_control_msg(serial->dev,                     /* usb device */
                                 usb_rcvctrlpipe(serial->dev, 0), /* endpoint pipe */
                                 XR_GETN_REG,                     /* request */
                                 USB_DIR_IN | USB_TYPE_VENDOR,    /* request_type */
                                 0,                               /* request value */
                                 reg | (block << 8),              /* index */
                                 value,                           /* data */
                                 1,                               /* size */
                                 5000);                           /* timeout */

        return result;
}


static void vizzini_disable(struct usb_serial_port *port)
{
        int block = port->block;

        vizzini_set_reg(port->serial, block, UART_ENABLE, 0);
        vizzini_set_reg(port->serial, URM_REG_BLOCK, URM_ENABLE_BASE + block, 0);
}


static void vizzini_enable(struct usb_serial_port *port)
{
        int block = port->block;

        vizzini_set_reg(port->serial, URM_REG_BLOCK, URM_ENABLE_BASE + block, URM_ENABLE_0_TX);
        vizzini_set_reg(port->serial, block, UART_ENABLE, UART_ENABLE_TX | UART_ENABLE_RX);
        vizzini_set_reg(port->serial, URM_REG_BLOCK, URM_ENABLE_BASE + block, URM_ENABLE_0_TX | URM_ENABLE_0_RX);
}


static void vizzini_loopback(struct usb_serial_port *port, int from)
{
	int block = port->block;
	int lb;

	switch (from)
	{
	case 0: lb = UART_LOOPBACK_CTL_RX_UART0; break;
	case 1: lb = UART_LOOPBACK_CTL_RX_UART1; break;
	case 2: lb = UART_LOOPBACK_CTL_RX_UART2; break;
	case 3: lb = UART_LOOPBACK_CTL_RX_UART3; break;
	default: return;
	}

	info("Internal loopback from %d\n", from);

	vizzini_disable(port);
	vizzini_set_reg(port->serial, block, UART_LOOPBACK_CTL, UART_LOOPBACK_CTL_ENABLE | lb);
	vizzini_enable(port);
}


struct vizzini_baud_rate
{
	unsigned int tx;
	unsigned int rx0;
	unsigned int rx1;
};

static struct vizzini_baud_rate vizzini_baud_rates[] = {
	{ 0x000, 0x000, 0x000 },
	{ 0x000, 0x000, 0x000 },
	{ 0x100, 0x000, 0x100 },
	{ 0x020, 0x400, 0x020 },
	{ 0x010, 0x100, 0x010 },
	{ 0x208, 0x040, 0x208 },
	{ 0x104, 0x820, 0x108 },
	{ 0x844, 0x210, 0x884 },
	{ 0x444, 0x110, 0x444 },
	{ 0x122, 0x888, 0x224 },
	{ 0x912, 0x448, 0x924 },
	{ 0x492, 0x248, 0x492 },
	{ 0x252, 0x928, 0x292 },
	{ 0X94A, 0X4A4, 0XA52 },
	{ 0X52A, 0XAA4, 0X54A },
	{ 0XAAA, 0x954, 0X4AA },
	{ 0XAAA, 0x554, 0XAAA },
	{ 0x555, 0XAD4, 0X5AA },
	{ 0XB55, 0XAB4, 0X55A },
	{ 0X6B5, 0X5AC, 0XB56 },
	{ 0X5B5, 0XD6C, 0X6D6 },
	{ 0XB6D, 0XB6A, 0XDB6 },
	{ 0X76D, 0X6DA, 0XBB6 },
	{ 0XEDD, 0XDDA, 0X76E },
	{ 0XDDD, 0XBBA, 0XEEE },
	{ 0X7BB, 0XF7A, 0XDDE },
	{ 0XF7B, 0XEF6, 0X7DE },
	{ 0XDF7, 0XBF6, 0XF7E },
	{ 0X7F7, 0XFEE, 0XEFE },
	{ 0XFDF, 0XFBE, 0X7FE },
	{ 0XF7F, 0XEFE, 0XFFE },
	{ 0XFFF, 0XFFE, 0XFFD },
};

static int vizzini_set_baud_rate(struct usb_serial_port *port, unsigned int rate)
{
	int 		block 	= port->block;
	unsigned int 	divisor = 48000000 / rate;
	unsigned int 	i 	= ((32 * 48000000) / rate) & 0x1f;
	unsigned int 	tx_mask = vizzini_baud_rates[i].tx;
	unsigned int 	rx_mask = (divisor & 1) ? vizzini_baud_rates[i].rx1 : vizzini_baud_rates[i].rx0;

	dbg("Setting baud rate to %d: i=%u div=%u tx=%03x rx=%03x\n", rate, i, divisor, tx_mask, rx_mask);

	vizzini_set_reg(port->serial, block, UART_CLOCK_DIVISOR_0, (divisor >>  0) & 0xff);
	vizzini_set_reg(port->serial, block, UART_CLOCK_DIVISOR_1, (divisor >>  8) & 0xff);
	vizzini_set_reg(port->serial, block, UART_CLOCK_DIVISOR_2, (divisor >> 16) & 0xff);
	vizzini_set_reg(port->serial, block, UART_TX_CLOCK_MASK_0, (tx_mask >>  0) & 0xff);
	vizzini_set_reg(port->serial, block, UART_TX_CLOCK_MASK_1, (tx_mask >>  8) & 0xff);
	vizzini_set_reg(port->serial, block, UART_RX_CLOCK_MASK_0, (rx_mask >>  0) & 0xff);
	vizzini_set_reg(port->serial, block, UART_RX_CLOCK_MASK_1, (rx_mask >>  8) & 0xff);

	return -EINVAL;
}


static void vizzini_set_termios(struct usb_serial_port *port,
                                struct termios *old_termios)
{
        unsigned int             cflag, block;
        speed_t                  rate;
        unsigned int             format_size, format_parity, format_stop, flow, gpio_mode;

        struct tty_struct       *tty = port->tty;

        dbg("%s\n", __func__);

        cflag = tty->termios->c_cflag;

        port->clocal = ((cflag & CLOCAL) != 0);

        block = port->block;

        vizzini_disable(port);

        if ((cflag & CSIZE) == CS7) {
                format_size = UART_FORMAT_SIZE_7;
        } else if ((cflag & CSIZE) == CS5) {
                /* Enabling 5-bit mode is really 9-bit mode! */
                format_size = UART_FORMAT_SIZE_9;
        } else {
                format_size = UART_FORMAT_SIZE_8;
        }
        port->trans9 = (format_size == UART_FORMAT_SIZE_9);

        if (cflag & PARENB) {
                if (cflag & PARODD) {
                        if (cflag & CMSPAR) {
                                format_parity = UART_FORMAT_PARITY_1;
                        } else {
                                format_parity = UART_FORMAT_PARITY_ODD;
                        }
                } else {
                        if (cflag & CMSPAR) {
                                format_parity = UART_FORMAT_PARITY_0;
                        } else {
                                format_parity = UART_FORMAT_PARITY_EVEN;
                        }
                }
        } else {
                format_parity = UART_FORMAT_PARITY_NONE;
        }

        if (cflag & CSTOPB) {
                format_stop = UART_FORMAT_STOP_2;
        } else {
                format_stop = UART_FORMAT_STOP_1;
        }

#ifdef VIZZINI_IWA
        if (format_size == UART_FORMAT_SIZE_8) {
                port->iwa = format_parity;
                if (port->iwa != UART_FORMAT_PARITY_NONE) {
                        format_size = UART_FORMAT_SIZE_9;
                        format_parity = UART_FORMAT_PARITY_NONE;
                }
        } else {
                port->iwa = UART_FORMAT_PARITY_NONE;
        }
#endif
        vizzini_set_reg(port->serial, block, UART_FORMAT, format_size | format_parity | format_stop);

        if (cflag & CRTSCTS) {
                flow      = UART_FLOW_MODE_HW;
                gpio_mode = UART_GPIO_MODE_SEL_RTS_CTS;
        } else if (I_IXOFF(tty) || I_IXON(tty)) {
                unsigned char   start_char = START_CHAR(tty);
                unsigned char   stop_char  = STOP_CHAR(tty);

                flow      = UART_FLOW_MODE_SW;
                gpio_mode = UART_GPIO_MODE_SEL_GPIO;

                vizzini_set_reg(port->serial, block, UART_XON_CHAR, start_char);
                vizzini_set_reg(port->serial, block, UART_XOFF_CHAR, stop_char);
        } else {
                flow      = UART_FLOW_MODE_NONE;
                gpio_mode = UART_GPIO_MODE_SEL_GPIO;
        }

        vizzini_set_reg(port->serial, block, UART_FLOW, flow);
        vizzini_set_reg(port->serial, block, UART_GPIO_MODE, gpio_mode);

        if (port->trans9) {
                /* Turn on wide mode if we're 9-bit transparent. */
                vizzini_set_reg(port->serial, EPLOCALS_REG_BLOCK, (block * MEM_EP_LOCALS_SIZE) + EP_WIDE_MODE, 1);
#ifdef VIZZINI_IWA
        } else if (port->iwa != UART_FORMAT_PARITY_NONE) {
                vizzini_set_reg(port->serial, EPLOCALS_REG_BLOCK, (block * MEM_EP_LOCALS_SIZE) + EP_WIDE_MODE, 1);
#endif
        } else if (!port->preciseflags) {
                /* Turn off wide mode unless we have precise flags. */
                vizzini_set_reg(port->serial, EPLOCALS_REG_BLOCK, (block * MEM_EP_LOCALS_SIZE) + EP_WIDE_MODE, 0);
        }

        rate = tty_get_baud_rate(tty);
	if(!rate) {
		rate = 9600;
	}
        vizzini_set_baud_rate(port, rate);

        vizzini_enable(port);

/*  mutex_unlock(&config_mutex); */
}


static void vizzini_break_ctl(struct usb_serial_port *port, int break_state)
{
        dbg("BREAK %d\n", break_state);
        if (break_state)
                acm_send_break(port, 0x10);
        else
                acm_send_break(port, 0x000);
}


static int vizzini_ioctl(struct usb_serial_port *port, struct file *file, unsigned int cmd, unsigned long arg)
{

        unsigned int             block, reg, val, match, preciseflags, unicast, broadcast, flow, selector, newctrl;
        char                    *data;
        int                      result;
        struct serial_struct     ss;

        dbg("%s %08x\n", __func__, cmd);

        switch (cmd) {
	case TIOCMGET:
		dbg (" (%d) TIOCMGET", port->number);
		return (port->ctrlout & ACM_CTRL_DTR ? TIOCM_DTR : 0) |
                (port->ctrlout & ACM_CTRL_RTS ? TIOCM_RTS : 0) |
                (port->ctrlin  & ACM_CTRL_DSR ? TIOCM_DSR : 0) |
                (port->ctrlin  & ACM_CTRL_RI  ? TIOCM_RI  : 0) |
                (port->ctrlin  & ACM_CTRL_DCD ? TIOCM_CD  : 0) |
                TIOCM_CTS;

	case TIOCMBIS:
		dbg(" (%d) TIOCMBIS",  port->number);
		if (copy_from_user(&val, (void *)arg, sizeof(int)))
			return -EFAULT;
		if (val & TIOCM_RTS)
			port->ctrlout |= ACM_CTRL_RTS;
		if (val & TIOCM_DTR)
			port->ctrlout |= ACM_CTRL_DTR;
		return acm_set_control(port, port->ctrlout);

	case TIOCMBIC:
		dbg(" (%d) TIOCMBIC",  port->number);
		if (copy_from_user(&val, (void *)arg, sizeof(int)))
			return -EFAULT;
		if (val & TIOCM_RTS)
			port->ctrlout &= ~ACM_CTRL_RTS;
		if (val & TIOCM_DTR)
			port->ctrlout &= ~ACM_CTRL_DTR;
		return acm_set_control(port, port->ctrlout);

	case TIOCMSET:
		dbg(" (%d) TIOCMSET",  port->number);
		if (copy_from_user(&val, (void *)arg, sizeof(int)))
			return -EFAULT;
		/* turn off RTS and DTR and then only turn
		on what was asked to */
		newctrl = ~(ACM_CTRL_RTS | ACM_CTRL_DTR);
		newctrl |= ((val & TIOCM_RTS) ? ACM_CTRL_RTS : 0);
		newctrl |= ((val & TIOCM_DTR) ? ACM_CTRL_DTR : 0);

		if (port->ctrlout == newctrl)
			return 0;
		return acm_set_control(port, port->ctrlout = newctrl);

        case TIOCGSERIAL:
		dbg("TIOCGSERIAL\n");
                if (!arg)
                        return -EFAULT;
                memset(&ss, 0, sizeof(ss));
                ss.baud_base = port->baud_base;
                if (copy_to_user((void *)arg, &ss, sizeof(ss)))
                        return -EFAULT;
                break;

        case TIOCSSERIAL:
		dbg("TIOCSSERIAL\n");
                if (!arg)
                        return -EFAULT;
                if (copy_from_user(&ss, (void *)arg, sizeof(ss)))
                        return -EFAULT;
                port->baud_base = ss.baud_base;
                dbg("baud_base=%d\n", port->baud_base);

                vizzini_disable(port);
                vizzini_set_baud_rate(port, port->baud_base);
                vizzini_enable(port);
                break;

        case VZIOC_GET_REG:
                if (get_user(block, (int *)arg))
                        return -EFAULT;
                if (get_user(reg, (int *)(arg + sizeof(int))))
                        return -EFAULT;

                data = kmalloc(1, GFP_KERNEL);
                if (data == NULL) {
                        err("%s - Cannot allocate USB buffer.\n", __func__);
                        return -ENOMEM;
                }

		if (block == -1)
			block = port->block;
                result = vizzini_get_reg(port->serial, block, reg, data);
                if (result != 1) {
                        err("Cannot get register (%d)\n", result);
                        kfree(data);
                        return -EFAULT;
                }

                if (put_user(data[0], (int *)(arg + 2 * sizeof(int)))) {
                        err("Cannot put user result\n");
                        kfree(data);
                        return -EFAULT;
                }

                kfree(data);
                break;

        case VZIOC_SET_REG:
                if (get_user(block, (int *)arg))
                        return -EFAULT;
                if (get_user(reg, (int *)(arg + sizeof(int))))
                        return -EFAULT;
                if (get_user(val, (int *)(arg + 2 * sizeof(int))))
                        return -EFAULT;

                result = vizzini_set_reg(port->serial, block, reg, val);
                if (result < 0)
                        return -EFAULT;
                break;

        case VZIOC_SET_ADDRESS_MATCH:
                match = arg;

                dbg("%s VIOC_SET_ADDRESS_MATCH %d\n", __func__, match);

                vizzini_disable(port);

                if (match & VZ_ADDRESS_MATCH_DISABLE) {
                        flow      = UART_FLOW_MODE_NONE;
                } else {
                        flow      = UART_FLOW_MODE_ADDR_MATCH_TX;
                        unicast   = (match >> VZ_ADDRESS_UNICAST_S) & 0xff;
                        broadcast = (match >> VZ_ADDRESS_BROADCAST_S) & 0xff;
                }

                dbg("address match: flow=%d ucast=%d bcast=%u\n",
                                   flow, unicast, broadcast);
                vizzini_set_reg(port->serial, port->block, UART_FLOW, flow);
                vizzini_set_reg(port->serial, port->block, UART_XON_CHAR, unicast);
                vizzini_set_reg(port->serial, port->block, UART_XOFF_CHAR, broadcast);

                vizzini_enable(port);
                break;

        case VZIOC_SET_PRECISE_FLAGS:
                preciseflags = arg;

                dbg("%s VIOC_SET_PRECISE_FLAGS %d\n", __func__, preciseflags);

                vizzini_disable(port);

                if (preciseflags) {
                        port->preciseflags = 1;
                } else {
                        port->preciseflags = 0;
                }

                vizzini_set_reg(port->serial, EPLOCALS_REG_BLOCK,
                                (port->block * MEM_EP_LOCALS_SIZE) + EP_WIDE_MODE,
                                port->preciseflags);

                vizzini_enable(port);
                break;

	case VZIOC_LOOPBACK:
		selector = arg;
		dbg("VIOC_LOOPBACK 0x%02x\n", selector);
		vizzini_loopback(port, selector);
		break;

        default:
		dbg("%s %08x - Unhandled ioctl\n", __func__, cmd);
                return -ENOIOCTLCMD;
        }

        return 0;
}


static int vizzini_open (struct usb_serial_port *port, struct file *filp)
{
	struct usb_serial *serial = port->serial;
	int result = 0;

	if (port_paranoia_check (port, __FUNCTION__))
		return -ENODEV;

	MOD_INC_USE_COUNT;

	dbg("%s - port %d", __FUNCTION__, port->number);

	down (&port->sem);

	++port->open_count;

	if (!port->active) {
		port->active = 1;

		/* force low_latency on so that our tty_push actually forces the data through,
		   otherwise it is scheduled, and with high data rates (like with OHCI) data
		   can get lost. */
		port->tty->low_latency = 1;

		/* if we have a bulk in, start reading from it */
		if (serial->num_bulk_in) {
			/* Start reading from the device */
			FILL_BULK_URB(port->read_urb, serial->dev,
				      usb_rcvbulkpipe(serial->dev, port->bulk_in_endpointAddress),
				      port->read_urb->transfer_buffer, port->read_urb->transfer_buffer_length,
				      vizzini_read_bulk_callback,
				      port);
			result = usb_submit_urb(port->read_urb);
			if (result)
				err("%s - failed resubmitting read urb, error %d", __FUNCTION__, result);
		}

		/* if we have a interrupt endpoint, start reading from it */
		if (serial->num_interrupt_in) {
			/* setup interrupt endpoint callback for the port */
			FILL_INT_URB(port->interrupt_in_urb, serial->dev,
				     usb_rcvintpipe(serial->dev, port->interrupt_in_endpointAddress),
				     port->interrupt_in_urb->transfer_buffer, port->interrupt_in_urb->transfer_buffer_length,
				     vizzini_int_callback,
				     port,
				     port->interrupt_in_interval);
			result = usb_submit_urb(port->interrupt_in_urb);
			if (result)
				err("%s - failed resubmitting interrupt urb, error %d", __FUNCTION__, result);
		}
	}

	up (&port->sem);

	return result;
}


static void vizzini_close (struct usb_serial_port *port, struct file * filp)
{
	struct usb_serial *serial = port->serial;

	dbg("%s - port %d", __FUNCTION__, port->number);

	--port->open_count;

	if (port->open_count <= 0) {
		if (serial->dev) {
			/* shutdown any bulk reads that might be going on */
			if (serial->num_bulk_out)
				usb_unlink_urb (port->write_urb);
			if (serial->num_bulk_in)
				usb_unlink_urb (port->read_urb);
			if (serial->num_interrupt_in)
				usb_unlink_urb (port->interrupt_in_urb);
		}

		port->active = 0;
		port->open_count = 0;
	}

	MOD_DEC_USE_COUNT;
}


static int vizzini_write (struct usb_serial_port *port, int from_user, const unsigned char *buf, int count)
{
	struct usb_serial *serial = port->serial;
	int result;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (count == 0) {
		dbg("%s - write request of 0 bytes", __FUNCTION__);
		return (0);
	}

	/* only do something if we have a bulk out endpoint */
	if (serial->num_bulk_out) {
		if (port->write_urb->status == -EINPROGRESS) {
			dbg ("%s - already writing", __FUNCTION__);
			return (0);
		}

		count = (count > port->bulk_out_size) ? port->bulk_out_size : count;

		if (from_user) {
			if (copy_from_user(port->write_urb->transfer_buffer, buf, count))
				return -EFAULT;
		}
		else {
			memcpy (port->write_urb->transfer_buffer, buf, count);
		}

		usb_serial_debug_data (__FILE__, __FUNCTION__, count, port->write_urb->transfer_buffer);

		/* set up our urb */
		FILL_BULK_URB(port->write_urb, serial->dev,
			      usb_sndbulkpipe(serial->dev, port->bulk_out_endpointAddress),
			      port->write_urb->transfer_buffer, count,
			       vizzini_write_bulk_callback,
			      port);

		/* send the data out the bulk port */
		result = usb_submit_urb(port->write_urb);
		if (result)
			err("%s - failed submitting write urb, error %d", __FUNCTION__, result);
		else
			result = count;

		return result;
	}

	/* no bulk out, so return 0 bytes written */
	return (0);
}


static int vizzini_write_room (struct usb_serial_port *port)
{
	struct usb_serial *serial = port->serial;
	int room = 0;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (serial->num_bulk_out) {
		if (port->write_urb->status != -EINPROGRESS)
			room = port->bulk_out_size;
	}

	dbg("%s - returns %d", __FUNCTION__, room);
	return (room);
}


static int vizzini_chars_in_buffer (struct usb_serial_port *port)
{
	struct usb_serial *serial = port->serial;
	int chars = 0;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (serial->num_bulk_out) {
		if (port->write_urb->status == -EINPROGRESS)
			chars = port->write_urb->transfer_buffer_length;
	}

	dbg ("%s - returns %d", __FUNCTION__, chars);
	return (chars);
}

static void vizzini_int_callback(struct urb *urb)
{
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
	struct tty_struct *tty = port->tty;
	struct usb_cdc_notification *dr = urb->transfer_buffer;

        unsigned char                   *data;
        int                              newctrl;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!serial) {
		dbg("%s - bad serial pointer, exiting", __FUNCTION__);
		return;
	}

	if (urb->status) {
		dbg("%s - nonzero read bulk status received: %d", __FUNCTION__, urb->status);
		return;
	}

        switch (urb->status) {
        case 0:
                /* success */
                break;
        case -ECONNRESET:
        case -ENOENT:
        case -ESHUTDOWN:
                /* this urb is terminated, clean up */
                dbg("urb shutting down with status: %d\n", urb->status);
                return;
        default:
                dbg("nonzero urb status received: %d\n", urb->status);
                return;
        }

        data = (unsigned char *)(dr + 1);
        switch (dr->bNotificationType) {

        case USB_CDC_NOTIFY_NETWORK_CONNECTION:
                dbg("%s network\n", dr->wValue ? "connected to" : "disconnected from");
                break;

        case USB_CDC_NOTIFY_SERIAL_STATE:
                newctrl = le16_to_cpu(get_unaligned((__u16 *)data));

                if (!port->clocal && (port->ctrlin & ~newctrl & ACM_CTRL_DCD)) {
                        dbg("calling hangup\n");
                        tty_hangup(tty);
                }

                port->ctrlin = newctrl;

                dbg("input control lines: dcd%c dsr%c break%c ring%c framing%c parity%c overrun%c\n",
			port->ctrlin & ACM_CTRL_DCD ? '+' : '-',
			port->ctrlin & ACM_CTRL_DSR ? '+' : '-',
			port->ctrlin & ACM_CTRL_BRK ? '+' : '-',
			port->ctrlin & ACM_CTRL_RI  ? '+' : '-',
			port->ctrlin & ACM_CTRL_FRAMING ? '+' : '-',
			port->ctrlin & ACM_CTRL_PARITY ? '+' : '-',
			port->ctrlin & ACM_CTRL_OVERRUN ? '+' : '-');
                break;

        default:
                dbg("unknown notification %d received: index %d len %d data0 %d data1 %d\n",
			dr->bNotificationType, dr->wIndex,
			dr->wLength, data[0], data[1]);
                break;
        }
}

static void vizzini_read_bulk_callback (struct urb *urb)
{
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
	struct tty_struct *tty;
	unsigned char *data = urb->transfer_buffer;
	int i;
	int result;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!serial) {
		dbg("%s - bad serial pointer, exiting", __FUNCTION__);
		return;
	}

	if (urb->status) {
		dbg("%s - nonzero read bulk status received: %d", __FUNCTION__, urb->status);
		return;
	}

	usb_serial_debug_data (__FILE__, __FUNCTION__, urb->actual_length, data);

	tty = port->tty;
	if (urb->actual_length) {
		for (i = 0; i < urb->actual_length ; ++i) {
			/* if we insert more than TTY_FLIPBUF_SIZE characters, we drop them. */
			if(tty->flip.count >= TTY_FLIPBUF_SIZE) {
				tty_flip_buffer_push(tty);
			}
			/* this doesn't actually push the data through unless tty->low_latency is set */
			tty_insert_flip_char(tty, data[i], 0);
		}
	  	tty_flip_buffer_push(tty);
	}

	/* Continue trying to always read  */
	FILL_BULK_URB(port->read_urb, serial->dev,
		      usb_rcvbulkpipe(serial->dev, port->bulk_in_endpointAddress),
		      port->read_urb->transfer_buffer, port->read_urb->transfer_buffer_length,
		      vizzini_read_bulk_callback,
		      port);
	result = usb_submit_urb(port->read_urb);
	if (result)
		err("%s - failed resubmitting read urb, error %d", __FUNCTION__, result);
}


static void vizzini_write_bulk_callback (struct urb *urb)
{
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!serial) {
		dbg("%s - bad serial pointer, exiting", __FUNCTION__);
		return;
	}

	if (urb->status) {
		dbg("%s - nonzero write bulk status received: %d", __FUNCTION__, urb->status);
		return;
	}

	queue_task(&port->tqueue, &tq_immediate);
	mark_bh(IMMEDIATE_BH);

	return;
}


static void vizzini_shutdown (struct usb_serial *serial)
{
	int i;

	dbg ("%s", __FUNCTION__);

	/* stop reads and writes on all ports */
	for (i=0; i < serial->num_ports; ++i) {
		while (serial->port[i].open_count > 0) {
			vizzini_close (&serial->port[i], NULL);
		}
	}
}


/*****************************************************************************
 * Driver tty interface functions
 *****************************************************************************/
static int serial_open (struct tty_struct *tty, struct file * filp)
{
	struct usb_serial *serial;
	struct usb_serial_port *port;
	int portNumber;

	dbg("%s", __FUNCTION__);

	/* initialize the pointer incase something fails */
	tty->driver_data = NULL;

	/* get the serial object associated with this tty pointer */
	serial = get_serial_by_minor (MINOR(tty->device));

	if (serial_paranoia_check (serial, __FUNCTION__)) {
		return -ENODEV;
	}

	/* set up our port structure making the tty driver remember our port object, and us it */
	portNumber = MINOR(tty->device) - serial->minor;
	port = &serial->port[portNumber];
	tty->driver_data = port;
	port->tty = tty;

	return (vizzini_open(port, filp));
}


static void serial_close(struct tty_struct *tty, struct file * filp)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial) {
		return;
	}

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->active) {
		dbg ("%s - port not opened", __FUNCTION__);
		return;
	}

	down (&port->sem);

	if (tty->driver_data == NULL) {
		/* disconnect beat us to the punch here, so handle it gracefully */
		goto exit;
	}

	vizzini_close(port, filp);
exit:
	up (&port->sem);
}


static int serial_write (struct tty_struct * tty, int from_user, const unsigned char *buf, int count)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial) {
		return -ENODEV;
	}

	dbg("%s - port %d, %d byte(s)", __FUNCTION__, port->number, count);

	if (!port->active) {
		dbg ("%s - port not opened", __FUNCTION__);
		return -EINVAL;
	}
	
	return (vizzini_write(port, from_user, buf, count));
}


static int serial_write_room (struct tty_struct *tty) 
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial) {
		return -ENODEV;
	}

	dbg("%s - port %d", __FUNCTION__, port->number);
	
	if (!port->active) {
		dbg ("%s - port not open", __FUNCTION__);
		return -EINVAL;
	}

	return (vizzini_write_room(port));
}


static int serial_chars_in_buffer (struct tty_struct *tty) 
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial) {
		return -ENODEV;
	}

	if (!port->active) {
		dbg ("%s - port not open", __FUNCTION__);
		return -EINVAL;
	}

	return (vizzini_chars_in_buffer(port));
}


static void serial_throttle (struct tty_struct * tty)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial) {
		return;
	}

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->active) {
		dbg ("%s - port not open", __FUNCTION__);
		return;
	}

	usb_unlink_urb (port->read_urb);
	return;
}


static void serial_unthrottle (struct tty_struct * tty)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
	
	int result;

	if (!serial) {
		return;
	}

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->active) {
		dbg ("%s - port not open", __FUNCTION__);
		return;
	}

	result = usb_submit_urb(port->read_urb);
	if (result)
		err("%s - failed submitting read urb, error %d", __FUNCTION__, result);

	return;
}


static int serial_ioctl (struct tty_struct *tty, struct file * file, unsigned int cmd, unsigned long arg)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial) {
		return -ENODEV;
	}

	dbg("%s - port %d, cmd 0x%.4x", __FUNCTION__, port->number, cmd);

	if (!port->active) {
		dbg ("%s - port not open", __FUNCTION__);
		return -ENODEV;
	}

	return (vizzini_ioctl(port, file, cmd, arg));
}


static void serial_set_termios (struct tty_struct *tty, struct termios * old)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial) {
		return;
	}

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->active) {
		dbg ("%s - port not open", __FUNCTION__);
		return;
	}

	vizzini_set_termios(port, old);

	return;
}


static void serial_break (struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial) {
		return;
	}

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->active) {
		dbg ("%s - port not open", __FUNCTION__);
		return;
	}

	vizzini_break_ctl(port, break_state);
}


static void serial_shutdown (struct usb_serial *serial)
{
	vizzini_shutdown(serial);
}


static void port_softint(void *private)
{
	struct usb_serial_port *port = (struct usb_serial_port *)private;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
	struct tty_struct *tty;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!serial) {
		return;
	}

	tty = port->tty;
	if ((tty->flags & (1 << TTY_DO_WRITE_WAKEUP)) && tty->ldisc.write_wakeup) {
		dbg("%s - write wakeup call.", __FUNCTION__);
		(tty->ldisc.write_wakeup)(tty);
	}

	wake_up_interruptible(&tty->write_wait);
}



static void * usb_serial_probe(struct usb_device *dev, unsigned int ifnum,
			       const struct usb_device_id *id)
{
	struct usb_serial *serial = NULL;
	struct usb_serial_port *port;
	struct usb_interface *interface;
	struct usb_interface_descriptor *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	struct usb_endpoint_descriptor *interrupt_in_endpoint = NULL;
	struct usb_endpoint_descriptor *bulk_in_endpoint = NULL;
	struct usb_endpoint_descriptor *bulk_out_endpoint = NULL;
	struct usb_serial_device_type *type = NULL;
	struct list_head *tmp;
	int found;
	int minor;
	int buffer_size;
	int i;
	char interrupt_pipe;
	char bulk_in_pipe;
	char bulk_out_pipe;
	int num_ports;
	int max_endpoints;
	const struct usb_device_id *id_pattern = NULL;

	/* loop through our list of known serial converters, and see if this
	   device matches. */
	found = 0;
	interface = &dev->actconfig->interface[ifnum];
	list_for_each (tmp, &usb_serial_driver_list) {
		type = list_entry(tmp, struct usb_serial_device_type, driver_list);
		id_pattern = usb_match_id(dev, interface, type->id_table);
		if (id_pattern != NULL) {
			dbg("descriptor matches");
			found = 1;
			break;
		}
	}
	if (!found) {
		/* no match */
		dbg("none matched");
		return(NULL);
	}

	/* descriptor matches, let's find the endpoints needed */
	interrupt_pipe = bulk_in_pipe = bulk_out_pipe = HAS_NOT;

	/* check out the endpoints */
	iface_desc = &interface->altsetting[0];
	for (i = 0; i < iface_desc->bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i];

		if ((endpoint->bEndpointAddress & 0x80) &&
		    ((endpoint->bmAttributes & 3) == 0x02)) {
			/* we found a bulk in endpoint */
			dbg("found bulk in");
			bulk_in_pipe = HAS;
			bulk_in_endpoint = endpoint;
		}

		if (((endpoint->bEndpointAddress & 0x80) == 0x00) &&
		    ((endpoint->bmAttributes & 3) == 0x02)) {
			/* we found a bulk out endpoint */
			dbg("found bulk out");
			bulk_out_pipe = HAS;
			bulk_out_endpoint = endpoint;
		}
	}

	/* verify that we found all of the endpoints that we need */
	if ((bulk_in_pipe == HAS) && (bulk_out_pipe == HAS)) {
		dbg("going to find the interrupt endpoint");
		interface = &dev->actconfig->interface[ifnum-1]; // the previous interface of data
		iface_desc = &interface->altsetting[0];
		endpoint = &iface_desc->endpoint[0];
		if ((endpoint->bEndpointAddress & 0x80) &&
		    ((endpoint->bmAttributes & 3) == 0x03)) {
			/* we found a interrupt in endpoint */
			dbg("found interrupt in");
			interrupt_pipe = HAS;
			interrupt_in_endpoint = endpoint;
		}
	}
	
	interface = &dev->actconfig->interface[ifnum];

	/* verify that we found all of the endpoints that we need */
	if (!((interrupt_pipe & type->needs_interrupt_in) &&
	      (bulk_in_pipe & type->needs_bulk_in) &&
	      (bulk_out_pipe & type->needs_bulk_out))) {
		/* nope, they don't match what we expected */
		dbg("descriptors matched, but endpoints (%d) did not.", iface_desc->bNumEndpoints);
		return NULL;
	}

	/* found all that we need */
	//info("%s converter detected", type->name);

	num_ports = 1;

	serial = get_free_serial (num_ports, &minor);
	if (serial == NULL) {
		err("No more free serial devices");
		return NULL;
	}

	serial->dev = dev;
	serial->type = type;
	serial->interface = interface;
	serial->minor = minor;
	serial->num_ports = num_ports;
	serial->num_bulk_in = 1;
	serial->num_bulk_out = 1;
	serial->num_interrupt_in = 1;
	serial->vendor = dev->descriptor.idVendor;
	serial->product = dev->descriptor.idProduct;

	/* if this device type has a startup function, call it */
	if (type->startup) {
		i = type->startup (serial);
		if (i < 0)
			goto probe_error;
		if (i > 0)
			return serial;
	}

	/* set up the endpoint information */
	endpoint = bulk_in_endpoint;
	port = &serial->port[0];
	port->read_urb = usb_alloc_urb (0);
	if (!port->read_urb) {
		err("No free urbs available");
		goto probe_error;
	}
	buffer_size = endpoint->wMaxPacketSize;
	port->bulk_in_endpointAddress = endpoint->bEndpointAddress;
	port->bulk_in_buffer = kmalloc (buffer_size, GFP_KERNEL);
	if (!port->bulk_in_buffer) {
		err("Couldn't allocate bulk_in_buffer");
		goto probe_error;
	}
	FILL_BULK_URB(port->read_urb, dev,
		usb_rcvbulkpipe(dev, endpoint->bEndpointAddress),
		port->bulk_in_buffer, buffer_size,
		vizzini_read_bulk_callback,
		port);

	endpoint = bulk_out_endpoint;
	port = &serial->port[0];
	port->write_urb = usb_alloc_urb(0);
	if (!port->write_urb) {
		err("No free urbs available");
		goto probe_error;
	}
	buffer_size = endpoint->wMaxPacketSize;
	port->bulk_out_size = buffer_size;
	port->bulk_out_endpointAddress = endpoint->bEndpointAddress;
	port->bulk_out_buffer = kmalloc (buffer_size, GFP_KERNEL);
	if (!port->bulk_out_buffer) {
		err("Couldn't allocate bulk_out_buffer");
		goto probe_error;
	}
	FILL_BULK_URB(port->write_urb, dev,
		usb_sndbulkpipe(dev, endpoint->bEndpointAddress),
		port->bulk_out_buffer, buffer_size,
		vizzini_write_bulk_callback,
		port);

	endpoint = interrupt_in_endpoint;
	port = &serial->port[0];
	port->interrupt_in_urb = usb_alloc_urb(0);
	if (!port->interrupt_in_urb) {
		err("No free urbs available");
		goto probe_error;
	}
	buffer_size = endpoint->wMaxPacketSize;
	port->interrupt_in_endpointAddress = endpoint->bEndpointAddress;
	port->interrupt_in_interval = endpoint->bInterval;
	port->interrupt_in_buffer = kmalloc (buffer_size, GFP_KERNEL);
	if (!port->interrupt_in_buffer) {
		err("Couldn't allocate interrupt_in_buffer");
		goto probe_error;
	}
	FILL_INT_URB(port->interrupt_in_urb, dev,
		usb_rcvintpipe(dev, endpoint->bEndpointAddress),
		port->interrupt_in_buffer, buffer_size,
		vizzini_int_callback,
		port,
		endpoint->bInterval);

	/* initialize some parts of the port structures */
	max_endpoints = max(1, (int)serial->num_ports);
	dbg ("%s - setting up port structures for this device", __FUNCTION__);
	for (i = 0; i < max_endpoints; ++i) {
		port = &serial->port[i];
		port->number = i + serial->minor;
		port->serial = serial;
		port->magic = USB_SERIAL_PORT_MAGIC;
		port->tqueue.routine = port_softint;
		port->tqueue.data = port;
		init_MUTEX (&port->sem);

		// store the vizzini block
		port->block = (ifnum-1)/2;
	}

	/* initialize the devfs nodes for this device and let the user know what ports we are bound to */
	for (i = 0; i < serial->num_ports; ++i) {
		tty_register_devfs (&serial_tty_driver, 0, serial->port[i].number);
		info("%s usb-uart attached to ttyUSB%d (or usb/tts/%d for devfs)",
		     type->name, serial->port[i].number, serial->port[i].number);
	}

	return serial; /* success */


probe_error:
	port = &serial->port[i];
	if (port->read_urb)
		usb_free_urb (port->read_urb);
	if (port->bulk_in_buffer)
		kfree (port->bulk_in_buffer);

	port = &serial->port[i];
	if (port->write_urb)
		usb_free_urb (port->write_urb);
	if (port->bulk_out_buffer)
		kfree (port->bulk_out_buffer);
	
	port = &serial->port[i];
	if (port->interrupt_in_urb)
		usb_free_urb (port->interrupt_in_urb);
	if (port->interrupt_in_buffer)
		kfree (port->interrupt_in_buffer);

	/* return the minor range that this device had */
	return_serial (serial);

	/* free up any memory that we allocated */
	kfree (serial);
	return NULL;
}


static void usb_serial_disconnect(struct usb_device *dev, void *ptr)
{
	struct usb_serial *serial = (struct usb_serial *) ptr;
	struct usb_serial_port *port;
	int i;

	if (serial) {
		/* fail all future close/read/write/ioctl/etc calls */
		for (i = 0; i < serial->num_ports; ++i) {
			down (&serial->port[i].sem);
			if (serial->port[i].tty != NULL)
				serial->port[i].tty->driver_data = NULL;
			up (&serial->port[i].sem);
		}

		serial->dev = NULL;
		serial_shutdown (serial);

		for (i = 0; i < serial->num_ports; ++i)
			serial->port[i].active = 0;

		for (i = 0; i < serial->num_bulk_in; ++i) {
			port = &serial->port[i];
			if (port->read_urb) {
				usb_unlink_urb (port->read_urb);
				usb_free_urb (port->read_urb);
			}
			if (port->bulk_in_buffer)
				kfree (port->bulk_in_buffer);
		}
		for (i = 0; i < serial->num_bulk_out; ++i) {
			port = &serial->port[i];
			if (port->write_urb) {
				usb_unlink_urb (port->write_urb);
				usb_free_urb (port->write_urb);
			}
			if (port->bulk_out_buffer)
				kfree (port->bulk_out_buffer);
		}
		for (i = 0; i < serial->num_interrupt_in; ++i) {
			port = &serial->port[i];
			if (port->interrupt_in_urb) {
				usb_unlink_urb (port->interrupt_in_urb);
				usb_free_urb (port->interrupt_in_urb);
			}
			if (port->interrupt_in_buffer)
				kfree (port->interrupt_in_buffer);
		}

		for (i = 0; i < serial->num_ports; ++i) {
			tty_unregister_devfs (&serial_tty_driver, serial->port[i].number);
			info("%s usb-uart now disconnected from ttyUSB%d", serial->type->name, serial->port[i].number);
		}

		/* return the minor range that this device had */
		return_serial (serial);

		/* free up any memory that we allocated */
		kfree (serial);

	} else {
		info("device disconnected");
	}

}

static __devinitdata struct usb_device_id id_table [] = {
        { USB_DEVICE(0x04e2, 0x1410) },
        { USB_DEVICE(0x04e2, 0x1412) },
        { USB_DEVICE(0x04e2, 0x1414) },
        { }
};
MODULE_DEVICE_TABLE(usb, id_table);

static struct usb_driver usb_serial_driver = {
	name:		"vizzini",
	probe:		usb_serial_probe,
	disconnect:	usb_serial_disconnect,
	id_table:	id_table, 			/* check all devices */
};

/* All of the device info needed for the vizzini Serial Converter */
static struct usb_serial_device_type vizzini_device = {
	name:			"vizzini",
	id_table:		id_table,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		1,
	shutdown:		vizzini_shutdown,
};


static struct tty_driver serial_tty_driver = {
	magic:			TTY_DRIVER_MAGIC,
	driver_name:		"vizzini",
	name:			"usb/tts/%d",
	major:			SERIAL_TTY_MAJOR,
	minor_start:		0,
	num:			SERIAL_TTY_MINORS,
	type:			TTY_DRIVER_TYPE_SERIAL,
	subtype:		SERIAL_TYPE_NORMAL,
	flags:			TTY_DRIVER_REAL_RAW | TTY_DRIVER_NO_DEVFS,

	refcount:		&serial_refcount,
	table:			serial_tty,
	termios:		serial_termios,
	termios_locked:		serial_termios_locked,

	open:			serial_open,
	close:			serial_close,
	write:			serial_write,
	write_room:		serial_write_room,
	ioctl:			serial_ioctl,
	set_termios:		serial_set_termios,
	throttle:		serial_throttle,
	unthrottle:		serial_unthrottle,
	break_ctl:		serial_break,
	chars_in_buffer:	serial_chars_in_buffer,
};


static int __init vizziniusb_serial_init(void)
{
	int i;
	int result;

	/* Initalize our global data */
	for (i = 0; i < SERIAL_TTY_MINORS; ++i) {
		serial_table[i] = NULL;
	}

	/* register the tty driver */
	serial_tty_driver.init_termios          = tty_std_termios;
	serial_tty_driver.init_termios.c_cflag  = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
	if (tty_register_driver (&serial_tty_driver)) {
		err("%s - failed to register tty driver", __FUNCTION__);
		return -1;
	}

	/* register the USB driver */
	result = usb_register(&usb_serial_driver);
	if (result < 0) {
		tty_unregister_driver(&serial_tty_driver);
		err("usb_register failed for the usb-serial driver. Error number %d", result);
		return -1;
	}

	usb_serial_register (&vizzini_device);

	info(DRIVER_DESC " " DRIVER_VERSION);

	return 0;
}


static void __exit vizziniusb_serial_exit(void)
{

	usb_serial_deregister (&vizzini_device);

	usb_deregister(&usb_serial_driver);
	tty_unregister_driver(&serial_tty_driver);
}

int usb_serial_register(struct usb_serial_device_type *new_device)
{
	/* Add this device to our list of devices */
	list_add(&new_device->driver_list, &usb_serial_driver_list);

//	info ("USB Serial support registered for %s", new_device->name);

	usb_scan_devices();

	return 0;
}


void usb_serial_deregister(struct usb_serial_device_type *device)
{
	struct usb_serial *serial;
	int i;

//	info("USB Serial deregistering driver %s", device->name);

	/* clear out the serial_table if the device is attached to a port */
	for(i = 0; i < SERIAL_TTY_MINORS; ++i) {
		serial = serial_table[i];
		if ((serial != NULL) && (serial->type == device)) {
			usb_driver_release_interface (&usb_serial_driver, serial->interface);
			usb_serial_disconnect (NULL, serial);
		}
	}

	list_del(&device->driver_list);
}

module_init(vizziniusb_serial_init);
module_exit(vizziniusb_serial_exit);


/* Module information */
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
//MODULE_VERSION( DRIVER_VERSION );
MODULE_LICENSE("GPL");

MODULE_PARM(debug, "i");
MODULE_PARM_DESC(debug, "Debug enabled or not");
