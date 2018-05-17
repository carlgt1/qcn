#ifndef VIZZINI_H
#define VIZZINI_H

#define XR_SET_REG              0
#define XR_GETN_REG             1

#define UART_0_REG_BLOCK        0
#define UART_1_REG_BLOCK        1
#define UART_2_REG_BLOCK        2
#define UART_3_REG_BLOCK        3
#define URM_REG_BLOCK           4
#define PRM_REG_BLOCK           5
#define EPMERR_REG_BLOCK        6
#define RAMCTL_REG_BLOCK        0x64
#define TWI_ROM_REG_BLOCK       0x65
#define EPLOCALS_REG_BLOCK      0x66

#define MEM_SHADOW_REG_SIZE_S   5
#define MEM_SHADOW_REG_SIZE     (1 << MEM_SHADOW_REG_SIZE_S)

#define MEM_EP_LOCALS_SIZE_S    3
#define MEM_EP_LOCALS_SIZE      (1 << MEM_EP_LOCALS_SIZE_S)

#define EP_WIDE_MODE            0x03


#define UART_GPIO_MODE                                     0x01a

#define UART_GPIO_MODE_SEL_M                               0x7
#define UART_GPIO_MODE_SEL_S                               0
#define UART_GPIO_MODE_SEL                                 0x007

#define UART_GPIO_MODE_SEL_GPIO                            (0x0 << UART_GPIO_MODE_SEL_S)
#define UART_GPIO_MODE_SEL_RTS_CTS                         (0x1 << UART_GPIO_MODE_SEL_S)
#define UART_GPIO_MODE_SEL_DTR_DSR                         (0x2 << UART_GPIO_MODE_SEL_S)
#define UART_GPIO_MODE_SEL_XCVR_EN_ACT                     (0x3 << UART_GPIO_MODE_SEL_S)
#define UART_GPIO_MODE_SEL_XCVR_EN_FLOW                    (0x4 << UART_GPIO_MODE_SEL_S)

#define UART_GPIO_MODE_XCVR_EN_POL_M                       0x1
#define UART_GPIO_MODE_XCVR_EN_POL_S                       3
#define UART_GPIO_MODE_XCVR_EN_POL                         0x008

#define UART_ENABLE                                        0x003
#define UART_ENABLE_TX_M                                   0x1
#define UART_ENABLE_TX_S                                   0
#define UART_ENABLE_TX                                     0x001
#define UART_ENABLE_RX_M                                   0x1
#define UART_ENABLE_RX_S                                   1
#define UART_ENABLE_RX                                     0x002

#define UART_CLOCK_DIVISOR_0                               0x004
#define UART_CLOCK_DIVISOR_1                               0x005
#define UART_CLOCK_DIVISOR_2                               0x006

#define UART_CLOCK_DIVISOR_2_MSB_M                         0x7
#define UART_CLOCK_DIVISOR_2_MSB_S                         0
#define UART_CLOCK_DIVISOR_2_MSB                           0x007
#define UART_CLOCK_DIVISOR_2_DIAGMODE_M                    0x1
#define UART_CLOCK_DIVISOR_2_DIAGMODE_S                    3
#define UART_CLOCK_DIVISOR_2_DIAGMODE                      0x008

#define UART_TX_CLOCK_MASK_0                               0x007
#define UART_TX_CLOCK_MASK_1                               0x008

#define UART_RX_CLOCK_MASK_0                               0x009
#define UART_RX_CLOCK_MASK_1                               0x00a

#define UART_FORMAT                                        0x00b

#define UART_FORMAT_SIZE_M                                 0xf
#define UART_FORMAT_SIZE_S                                 0
#define UART_FORMAT_SIZE                                   0x00f

#define UART_FORMAT_SIZE_7                                 (0x7 << UART_FORMAT_SIZE_S)
#define UART_FORMAT_SIZE_8                                 (0x8 << UART_FORMAT_SIZE_S)
#define UART_FORMAT_SIZE_9                                 (0x9 << UART_FORMAT_SIZE_S)

#define UART_FORMAT_PARITY_M                               0x7
#define UART_FORMAT_PARITY_S                               4
#define UART_FORMAT_PARITY                                 0x070

#define UART_FORMAT_PARITY_NONE                            (0x0 << UART_FORMAT_PARITY_S)
#define UART_FORMAT_PARITY_ODD                             (0x1 << UART_FORMAT_PARITY_S)
#define UART_FORMAT_PARITY_EVEN                            (0x2 << UART_FORMAT_PARITY_S)
#define UART_FORMAT_PARITY_1                               (0x3 << UART_FORMAT_PARITY_S)
#define UART_FORMAT_PARITY_0                               (0x4 << UART_FORMAT_PARITY_S)

#define UART_FORMAT_STOP_M                                 0x1
#define UART_FORMAT_STOP_S                                 7
#define UART_FORMAT_STOP                                   0x080

#define UART_FORMAT_STOP_1                                 (0x0 << UART_FORMAT_STOP_S)
#define UART_FORMAT_STOP_2                                 (0x1 << UART_FORMAT_STOP_S)

#define UART_FORMAT_MODE_7N1                               0
#define UART_FORMAT_MODE_RES1                              1
#define UART_FORMAT_MODE_RES2                              2
#define UART_FORMAT_MODE_RES3                              3
#define UART_FORMAT_MODE_7N2                               4
#define UART_FORMAT_MODE_7P1                               5
#define UART_FORMAT_MODE_8N1                               6
#define UART_FORMAT_MODE_RES7                              7
#define UART_FORMAT_MODE_7P2                               8
#define UART_FORMAT_MODE_8N2                               9
#define UART_FORMAT_MODE_8P1                               10
#define UART_FORMAT_MODE_9N1                               11
#define UART_FORMAT_MODE_8P2                               12
#define UART_FORMAT_MODE_RESD                              13
#define UART_FORMAT_MODE_RESE                              14
#define UART_FORMAT_MODE_9N2                               15

#define UART_FLOW                                          0x00c

#define UART_FLOW_MODE_M                                   0x7
#define UART_FLOW_MODE_S                                   0
#define UART_FLOW_MODE                                     0x007

#define UART_FLOW_MODE_NONE                                (0x0 << UART_FLOW_MODE_S)
#define UART_FLOW_MODE_HW                                  (0x1 << UART_FLOW_MODE_S)
#define UART_FLOW_MODE_SW                                  (0x2 << UART_FLOW_MODE_S)
#define UART_FLOW_MODE_ADDR_MATCH                          (0x3 << UART_FLOW_MODE_S)
#define UART_FLOW_MODE_ADDR_MATCH_TX                       (0x4 << UART_FLOW_MODE_S)

#define UART_FLOW_HALF_DUPLEX_M                            0x1
#define UART_FLOW_HALF_DUPLEX_S                            3
#define UART_FLOW_HALF_DUPLEX                              0x008

#define UART_LOOPBACK_CTL                                  0x012
#define UART_LOOPBACK_CTL_ENABLE_M                         0x1
#define UART_LOOPBACK_CTL_ENABLE_S                         2
#define UART_LOOPBACK_CTL_ENABLE                           0x004
#define UART_LOOPBACK_CTL_RX_SOURCE_M                      0x3
#define UART_LOOPBACK_CTL_RX_SOURCE_S                      0
#define UART_LOOPBACK_CTL_RX_SOURCE                        0x003
#define UART_LOOPBACK_CTL_RX_UART0                         (0x0 << UART_LOOPBACK_CTL_RX_SOURCE_S)
#define UART_LOOPBACK_CTL_RX_UART1                         (0x1 << UART_LOOPBACK_CTL_RX_SOURCE_S)
#define UART_LOOPBACK_CTL_RX_UART2                         (0x2 << UART_LOOPBACK_CTL_RX_SOURCE_S)
#define UART_LOOPBACK_CTL_RX_UART3                         (0x3 << UART_LOOPBACK_CTL_RX_SOURCE_S)

#define UART_CHANNEL_NUM                                   0x00d

#define UART_XON_CHAR                                      0x010
#define UART_XOFF_CHAR                                     0x011

#define UART_GPIO_SET                                      0x01d
#define UART_GPIO_CLR                                      0x01e
#define UART_GPIO_STATUS                                   0x01f

#define URM_ENABLE_BASE                                    0x010
#define URM_ENABLE_0                                       0x010
#define URM_ENABLE_0_TX_M                                  0x1
#define URM_ENABLE_0_TX_S                                  0
#define URM_ENABLE_0_TX                                    0x001
#define URM_ENABLE_0_RX_M                                  0x1
#define URM_ENABLE_0_RX_S                                  1
#define URM_ENABLE_0_RX                                    0x002

#define URM_RX_FIFO_RESET_0                                0x018
#define URM_RX_FIFO_RESET_1                                0x019
#define URM_RX_FIFO_RESET_2                                0x01a
#define URM_RX_FIFO_RESET_3                                0x01b
#define URM_TX_FIFO_RESET_0                                0x01c
#define URM_TX_FIFO_RESET_1                                0x01d
#define URM_TX_FIFO_RESET_2                                0x01e
#define URM_TX_FIFO_RESET_3                                0x01f


#define RAMCTL_REGS_TXFIFO_0_LEVEL                         0x000
#define RAMCTL_REGS_TXFIFO_1_LEVEL                         0x001
#define RAMCTL_REGS_TXFIFO_2_LEVEL                         0x002
#define RAMCTL_REGS_TXFIFO_3_LEVEL                         0x003
#define RAMCTL_REGS_RXFIFO_0_LEVEL                         0x004

#define RAMCTL_REGS_RXFIFO_0_LEVEL_LEVEL_M                 0x7ff
#define RAMCTL_REGS_RXFIFO_0_LEVEL_LEVEL_S                 0
#define RAMCTL_REGS_RXFIFO_0_LEVEL_LEVEL                   0x7ff
#define RAMCTL_REGS_RXFIFO_0_LEVEL_STALE_M                 0x1
#define RAMCTL_REGS_RXFIFO_0_LEVEL_STALE_S                 11
#define RAMCTL_REGS_RXFIFO_0_LEVEL_STALE                   0x800

#define RAMCTL_REGS_RXFIFO_1_LEVEL                         0x005
#define RAMCTL_REGS_RXFIFO_2_LEVEL                         0x006
#define RAMCTL_REGS_RXFIFO_3_LEVEL                         0x007

#define RAMCTL_BUFFER_PARITY                               0x1
#define RAMCTL_BUFFER_BREAK                                0x2
#define RAMCTL_BUFFER_FRAME                                0x4
#define RAMCTL_BUFFER_OVERRUN                              0x8

#endif /*VIZZINI_H*/

#define VIZZINI_IOC_MAGIC       	'v'

#define VZIOC_GET_REG           	_IOWR(VIZZINI_IOC_MAGIC, 1, int)
#define VZIOC_SET_REG           	_IOWR(VIZZINI_IOC_MAGIC, 2, int)
#define VZIOC_SET_ADDRESS_MATCH 	_IO(VIZZINI_IOC_MAGIC, 3)
#define VZIOC_SET_PRECISE_FLAGS     	_IO(VIZZINI_IOC_MAGIC, 4)
#define VZIOC_TEST_MODE         	_IO(VIZZINI_IOC_MAGIC, 5)
#define VZIOC_LOOPBACK          	_IO(VIZZINI_IOC_MAGIC, 6)

#define VZ_ADDRESS_UNICAST_S        	0
#define VZ_ADDRESS_BROADCAST_S      	8
#define VZ_ADDRESS_MATCH(U, B)          (0x8000000 | ((B) << VZ_ADDRESS_BROADCAST_S) | ((U) << VZ_ADDRESS_UNICAST_S))
#define VZ_ADDRESS_MATCH_DISABLE    	0

#ifndef __LINUX_USB_SERIAL_H
#define __LINUX_USB_SERIAL_H

#include <linux/config.h>

#define SERIAL_TTY_MAJOR	188	/* Nice legal number now */
#define SERIAL_TTY_MINORS	255	/* loads of devices :) */

#define MAX_NUM_PORTS		8	/* The maximum number of ports one device can grab at once */

#define USB_SERIAL_MAGIC	0x6702	/* magic number for usb_serial struct */
#define USB_SERIAL_PORT_MAGIC	0x7301	/* magic number for usb_serial_port struct */

/* parity check flag */
#define RELEVANT_IFLAG(iflag)	(iflag & (IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK))

struct usb_serial_port {
	int			magic;
	struct usb_serial	*serial;	/* pointer back to the owner of this port */
	struct tty_struct *	tty;		/* the coresponding tty for this port */
	unsigned char		number;
	char			active;		/* someone has this device open */

	unsigned char *		interrupt_in_buffer;
	struct urb *		interrupt_in_urb;
	__u8			interrupt_in_endpointAddress;
	unsigned int		interrupt_in_interval;	

	unsigned char *		bulk_in_buffer;
	struct urb *		read_urb;
	__u8			bulk_in_endpointAddress;

	unsigned char *		bulk_out_buffer;
	int			bulk_out_size;
	struct urb *		write_urb;
	__u8			bulk_out_endpointAddress;

	wait_queue_head_t	write_wait;

	struct tq_struct	tqueue;		/* task queue for line discipline waking up */
	int			open_count;	/* number of times this port has been opened */
	struct semaphore	sem;		/* locks this structure */

	void *			private;	/* data private to the specific port */

	int           ctrlin;
        int           ctrlout;
        int           clocal;

        int           block;
        int           preciseflags; /* USB: wide mode, TTY: flags per character */
        int           trans9;   /* USB: wide mode, serial 9N1 */
        unsigned int  baud_base; /* setserial: used to hack in non-standard baud rates */
        int           have_extra_byte;
        int           extra_byte;

	int           bcd_device;

#ifdef VIZZINI_IWA
        int           iwa;
#endif
};

struct usb_serial {
	int				magic;
	struct usb_device *		dev;
	struct usb_serial_device_type *	type;			/* the type of usb serial device this is */
	struct usb_interface *		interface;		/* the interface for this device */
	struct tty_driver *		tty_driver;		/* the tty_driver for this device */
	unsigned char			minor;			/* the starting minor number for this device */
	unsigned char			num_ports;		/* the number of ports this device has */
	char				num_interrupt_in;	/* number of interrupt in endpoints we have */
	char				num_bulk_in;		/* number of bulk in endpoints we have */
	char				num_bulk_out;		/* number of bulk out endpoints we have */
	__u16				vendor;			/* vendor id of this device */
	__u16				product;		/* product id of this device */
	struct usb_serial_port		port[MAX_NUM_PORTS];

	void *			private;		/* data private to the specific driver */
};


#define MUST_HAVE_NOT	0x01
#define MUST_HAVE	0x02
#define DONT_CARE	0x03

#define	HAS		0x02
#define HAS_NOT		0x01

#define NUM_DONT_CARE	(-1)


/* This structure defines the individual serial converter. */
struct usb_serial_device_type {
	char	*name;
	const struct usb_device_id *id_table;
	char	needs_interrupt_in;
	char	needs_bulk_in;
	char	needs_bulk_out;
	char	num_interrupt_in;
	char	num_bulk_in;
	char	num_bulk_out;
	char	num_ports;		/* number of serial ports this device has */

	struct list_head	driver_list;

	/* function call to make before accepting driver
	 * return 0 to continue initialization,
	 * < 0 aborts startup,
	 * > 0 does not set up anything else and is useful for devices that have
	 * downloaded firmware, and will reset themselves shortly.
	 */
	int (*startup) (struct usb_serial *serial);

	void (*shutdown) (struct usb_serial *serial);

	/* serial function calls */
	int  (*open)		(struct usb_serial_port *port, struct file * filp);
	void (*close)		(struct usb_serial_port *port, struct file * filp);
	int  (*write)		(struct usb_serial_port *port, int from_user, const unsigned char *buf, int count);
	int  (*write_room)	(struct usb_serial_port *port);
	int  (*ioctl)		(struct usb_serial_port *port, struct file * file, unsigned int cmd, unsigned long arg);
	void (*set_termios)	(struct usb_serial_port *port, struct termios * old);
	void (*break_ctl)	(struct usb_serial_port *port, int break_state);
	int  (*chars_in_buffer)	(struct usb_serial_port *port);
	void (*throttle)	(struct usb_serial_port *port);
	void (*unthrottle)	(struct usb_serial_port *port);

	void (*read_int_callback)(struct urb *urb);
	void (*read_bulk_callback)(struct urb *urb);
	void (*write_bulk_callback)(struct urb *urb);
};

extern int  usb_serial_register(struct usb_serial_device_type *new_device);
extern void usb_serial_deregister(struct usb_serial_device_type *device);

/* Inline functions to check the sanity of a pointer that is passed to us */
static inline int serial_paranoia_check (struct usb_serial *serial, const char *function)
{
	if (!serial) {
		dbg("%s - serial == NULL", function);
		return -1;
	}
	if (serial->magic != USB_SERIAL_MAGIC) {
		dbg("%s - bad magic number for serial", function);
		return -1;
	}
	if (!serial->type) {
		dbg("%s - serial->type == NULL!", function);
		return -1;
	}

	return 0;
}


static inline int port_paranoia_check (struct usb_serial_port *port, const char *function)
{
	if (!port) {
		dbg("%s - port == NULL", function);
		return -1;
	}
	if (port->magic != USB_SERIAL_PORT_MAGIC) {
		dbg("%s - bad magic number for port", function);
		return -1;
	}
	if (!port->serial) {
		dbg("%s - port->serial == NULL", function);
		return -1;
	}
	if (!port->tty) {
		dbg("%s - port->tty == NULL", function);
		return -1;
	}

	return 0;
}


static inline struct usb_serial* get_usb_serial (struct usb_serial_port *port, const char *function)
{
	/* if no port was specified, or it fails a paranoia check */
	if (!port ||
		port_paranoia_check (port, function) ||
		serial_paranoia_check (port->serial, function)) {
		/* then say that we dont have a valid usb_serial thing, which will
		 * end up genrating -ENODEV return values */
		return NULL;
	}

	return port->serial;
}


static inline void usb_serial_debug_data (const char *file, const char *function, int size, const unsigned char *data)
{
	int i;

	if (!debug)
		return;

	printk (KERN_DEBUG "%s: %s - length = %d, data = ", file, function, size);
	for (i = 0; i < size; ++i) {
		printk ("%.2x ", data[i]);
	}
	printk ("\n");
}

struct usb_cdc_notification {
	__u8	bmRequestType;
	__u8	bNotificationType;
	__u16	wValue;
	__u16	wIndex;
	__u16	wLength;
}__attribute__((packed));


/* Use our own dbg macro */
#undef dbg
//#define dbg(format, arg...) do { if (debug) printk(KERN_DEBUG "%s: " format "\n" , __FILE__ , ## arg); } while (0)
#define dbg(format, arg...) do { if (debug) info(KERN_DEBUG "%s: " format "\n" , __FILE__ , ## arg); } while (0)

#endif	/* ifdef __LINUX_USB_SERIAL_H */
