#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

// structure to store our device state
struct usb_st {
    struct usb_device *udev;
    struct usb_interface *interface;
    struct urb *bulk_in_urb;
    unsigned char *bulk_in_buffer;
    size_t bulk_in_size;
    __u8 bulk_in_endpointAddr;
    __u8 bulk_out_endpointAddr;
};

static int san_probe(struct usb_interface *interface, const struct usb_device_id *device_id)
{
	int retval;
    struct usb_st *dev;
    struct usb_endpoint_descriptor *bulkin, *bulkout;

    printk(KERN_INFO "san-driver: SanDisk USB (%04X:%04X) plugged in\n", device_id->idVendor, device_id->idProduct);

    // allocate memory for our device state and initialize it
    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;
    
    dev->udev = interface_to_usbdev(interface);
    dev->interface = interface;
    
    retval = usb_find_common_endpoints(interface->cur_altsetting,
            &bulkin, &bulkout, NULL, NULL);
    
    if (retval) {
        dev_err(&interface->dev,
                "Could not find both bulk-in and bulk-out endpoints\n");
        goto error;
    }
    
    dev->bulk_in_size = usb_endpoint_maxp(bulkin);
    

error:
	return retval;
}

static void san_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "san-driver: SanDisk USB disconnected\n");
}

static struct usb_device_id san_table[] =
{
	{ USB_DEVICE(0x0781, 0x5567) },
	{}
};

static struct usb_driver san_driver =
{
	.name		= "san-driver",
	.id_table	= san_table,
	.probe		= san_probe,
	.disconnect	= san_disconnect,
};

MODULE_DEVICE_TABLE(usb, san_table);

static int __init san_init(void)
{
	return usb_register(&san_driver);
}

static void __exit san_exit(void)
{
	usb_deregister(&san_driver);
}

module_init(san_init);
module_exit(san_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanjit SenGupta");
MODULE_DESCRIPTION("SanDisk USB Driver");
