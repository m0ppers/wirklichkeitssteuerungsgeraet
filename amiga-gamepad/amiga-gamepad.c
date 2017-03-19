/**
  @file usbd_hid_test.c
  @brief
  USB HID Device Test

  This example program will send a string of data as HID reports to the host
  on the USB.
 */
/*
 * ============================================================================
 * History
 * =======
 * GDM 21 Jan 2015 : Created
 *
 * (C) Copyright 2016, Future Technology Devices International Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Future Technology Devices
 * International Limited ("FTDI") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of FTDI liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on FTDI's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, FTDI
 * has no liability in relation to those amendments.
 * ============================================================================
 */

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include <ft900.h>
#include <ft900_gpio.h>
#include <ft900_usb.h>
#include <ft900_usb_hid.h>

/* UART support for printf output. */
#include "tinyprintf/tinyprintf.h"

/* CONSTANTS ***********************************************************************/

#define USB_CONFIG_BMATTRIBUTES_VALUE USB_CONFIG_BMATTRIBUTES_RESERVED_SET_TO_1
/// USB Endpoint Zero packet size (both must match)
#define USB_CONTROL_EP_MAX_PACKET_SIZE 64
#define USB_CONTROL_EP_SIZE USBD_EP_SIZE_64
//@}

/**
 @name Device Configuration Areas
 @brief Size and location reserved for string descriptors.
 Leaving the allocation size blank will make an array exactly the size
 of the string allocation.
 Note: Specifying the location is not supported by the GCC compiler.
 */
//@{
// String descriptors - allow a maximum of 256 bytes for this
#define STRING_DESCRIPTOR_LOCATION 0x80
#define STRING_DESCRIPTOR_ALLOCATION 0x100
//@}

/**
 @name USB_PID_KEYBOARD configuration.
 @brief Run Time Product ID for keyboard function.
 */
//@{
#define USB_PID_KEYBOARD 0x0fda
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

#define GPIO_FORWARD_PIN 30
#define GPIO_BACK_PIN 27
#define GPIO_LEFT_PIN 33
#define GPIO_RIGHT_PIN 5
#define GPIO_FIRE_PIN 29
#define GPIO_FIRE2_PIN 6


/**
 @name string_descriptor
 @brief Table of USB String descriptors
 */
const uint8_t string_descriptor[STRING_DESCRIPTOR_ALLOCATION] =
{
		// String 0 is actually a list of language IDs supported by the real strings.
		0x04, USB_DESCRIPTOR_TYPE_STRING, 0x09,
		0x04, // 0409 = English (US)

		0x28, USB_DESCRIPTOR_TYPE_STRING, 'A', 0x00, 'k', 0x00, 'r', 0x00, 'o',
		0x00, 'n', 0x00, 'y', 0x00, 'm', 0x00, 'e', 0x00, ' ', 0x00, 'A', 0x00,
		'n', 0x00, 'a', 0x00, 'l', 0x00, 'o', 0x00, 'g', 0x00, 'i', 0x00, 'k',
		0x00, 'e', 0x00, 'r', 0x00,

		// String 2 (Product): "AAMIIIGAAA Joystick"
		0x28, USB_DESCRIPTOR_TYPE_STRING, 'A', 0x00, 'A', 0x00, 'M', 0x00, 'I',
		0x00, 'I', 0x00, 'I', 0x00, 'G', 0x00, 'A', 0x00, 'A', 0x00, 'A', 0x00,
		' ', 0x00, 'J', 0x00, 'o', 0x00, 'y', 0x00, 's', 0x00, 't', 0x00, 'i',
		0x00, 'c', 0x00, 'k', 0x00,

		// String 3 (Serial Number): "Revision 2017"
		0x1C, USB_DESCRIPTOR_TYPE_STRING, 'R', 0x00, 'e', 0x00, 'v', 0x00, 'i',
		0x00, 's', 0x00, 'i', 0x00, 'o', 0x00, 'n', 0x00, ' ', 0x00, '2', 0x00,
		'0', 0x00, '1', 0x00, '7', 0x00,

		// END OF STRINGS
		0x00
};
/**
 @name device_descriptor_keyboard
 @brief Device descriptor for Run Time mode.
 */
const USB_device_descriptor device_descriptor_keyboard =
{
		0x12, /* bLength */
		USB_DESCRIPTOR_TYPE_DEVICE, /* bDescriptorType */
		USB_BCD_VERSION_2_0, /* bcdUSB */          // V2.0
		USB_CLASS_DEVICE, /* bDeviceClass */       // Defined in interface
		USB_SUBCLASS_DEVICE, /* bDeviceSubClass */ // Defined in interface
		USB_PROTOCOL_DEVICE, /* bDeviceProtocol */ // Defined in interface
		USB_CONTROL_EP_MAX_PACKET_SIZE, /* bMaxPacketSize0 */
		USB_VID_FTDI, /* idVendor */   // idVendor: 0x0403 (FTDI)
		USB_PID_KEYBOARD, /* idProduct */ // idProduct: 0x0fea
		0x0102, /* bcdDevice */        // 1.1
		0x01, /* iManufacturer */      // Manufacturer
		0x02, /* iProduct */           // Product
		0x03, /* iSerialNumber */      // Serial Number
		0x01, /* bNumConfigurations */
};

/**
 @name config_descriptor_keyboard
 @brief Configuration descriptor for Run Time mode.
 */
//@{
struct PACK config_descriptor_keyboard
{
	USB_configuration_descriptor configuration;
	USB_interface_descriptor interface;
	USB_hid_descriptor hid;
	USB_endpoint_descriptor endpoint;
};

const uint8_t hid_report_descriptor[] = {
	0x05, 0x01, //	USAGE_PAGE (Generic Desktop)
	0x09, 0x05, //	USAGE (Gamepad)
	0xA1, 0x01, //	COLLECTION (Application)
	0xa1, 0x00,  /*     COLLECTION (Physical)          */
	0x05, 0x01,  /*       USAGE_PAGE (Generic Desktop) */
	0x09, 0x30,  /*       USAGE (X)                    */
	0x09, 0x31,  /*       USAGE (Y)                    */
	0x15, 0x81,  /*       LOGICAL_MINIMUM (-127)          */
	0x25, 0x7f,  /*       LOGICAL_MAXIMUM (127)        */
	0x75, 0x08,  /*       REPORT_SIZE (8)              */
	0x95, 0x02,  /*       REPORT_COUNT (2)             */
	0x81, 0x02,  /*       INPUT (Data,Var,Rel)         */
	0xc0,        /*     END_COLLECTION                 */
	0x05, 0x09, //	USAGE_PAGE (Button)
	0x19, 0x01, //	USAGE_MINIMUM (Button 1)
	0x29, 0x02, //	USAGE_MAXIMUM (Button 8)
	0x15, 0x00, //	LOGICAL_MINIMUM (0)
	0x25, 0x01, //	LOGICAL_MAXIMUM (1)
	0x75, 0x01, //	REPORT_SIZE (1)
	0x95, 0x08, //	REPORT_COUNT (8)
	0x81, 0x02, //	INPUT (Data,Var,Abs)
	0xC0,		//	END_COLLECTION
};

const struct config_descriptor_keyboard config_descriptor_keyboard =
{
	{
		0x09, /* configuration.bLength */
		USB_DESCRIPTOR_TYPE_CONFIGURATION, /* configuration.bDescriptorType */
		sizeof(struct config_descriptor_keyboard), /* configuration.wTotalLength */
		0x01,
		0x01, /* configuration.bConfigurationValue */
		0x00, /* configuration.iConfiguration */
		USB_CONFIG_BMATTRIBUTES_VALUE, /* configuration.bmAttributes */
		0xFA, /* configuration.bMaxPower */           // 500mA
	},
	{
		// ---- INTERFACE DESCRIPTOR for Keyboard ----
		0x09, /* interface.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* interface.bDescriptorType */
		0, /* interface.bInterfaceNumber */
		0x00, /* interface.bAlternateSetting */
		0x01, /* interface.bNumEndpoints */
		USB_CLASS_HID, /* interface.bInterfaceClass */ // HID Class
		0, /* interface.bInterfaceSubClass */ // Boot Protocol
		0, /* interface.bInterfaceProtocol */ // Keyboard
		0x02, /* interface.iInterface */ // string descriptor index
	},
	{
		// ---- HID DESCRIPTOR for Keyboard ----
		0x09, /* hid.bLength */
		USB_DESCRIPTOR_TYPE_HID, /* hid.bDescriptorType */
		USB_BCD_VERSION_HID_1_1, /* hid.bcdHID */
		USB_HID_LANG_NOT_SUPPORTED, /* hid.bCountryCode */
		0x01, /* hid.bNumDescriptors */
		USB_DESCRIPTOR_TYPE_REPORT, /* hid.bDescriptorType_0 */
		sizeof(hid_report_descriptor), /* hid.wDescriptorLength_0 */
	},
	{
		// ---- ENDPOINT DESCRIPTOR for Keyboard ----
		0x07, /* endpoint.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_IN | 1, /* endpoint.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT, /* endpoint.bmAttributes */
		0x0008, /* endpoint.wMaxPacketSize */
		0x0a, /* endpoint.bInterval */
	},
};
//@}


//@}

/**
 @name report_protocol
 @brief Flag to set report protocol (0 - boot, 1 - report)
 */
static uint8_t report_protocol = 0;

/**
 @name report_enable
 @brief Flag to enable report mode (i.e. reporting active).
 */
static uint8_t report_enable = 0;

/**
 @name report_idle
 @brief Timer to set idle report period.
 */
static uint8_t report_idle = 0;

/**
 @name input_report
 @brief Flag to enable input reports (i.e. LEDs).
 */
static uint8_t input_report = 0;

/**
 @name keypress_counter
 @brief Count up ms timer to simulate keypress reports at a set interval.
 */
static volatile int16_t keypress_counter = 0;

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/** @name tfp_putc
 *  @details Machine dependent putc function for tfp_printf (tinyprintf) library.
 *  @param p Parameters (machine dependent)
 *  @param c The character to write
 */
void tfp_putc(void* p, char c)
{
	uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}

static uint8_t counter = 0;
typedef struct PACK JoystickState {
	int8_t lr;
	int8_t fb;
	uint8_t buttons;
//	unsigned int dummy2: 6;
//	unsigned int fire: 1;
//	unsigned int fire2: 1;
} JoystickState;

static JoystickState state;

void ISR_timer(void)
{
	if (timer_is_interrupted(timer_select_a)) {
		// Call USB timer handler to action transaction and hub timers.
		USBD_timer();
	}
}

/**
 @brief      USB class request handler
 @details    Handle class requests from the host application.
 The bRequest value is parsed and the appropriate
 action or function is performed. Additional values
 from the USB_device_request structure are decoded
 and the DFU state machine and status updated as
 required.
 @param[in]        req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return                status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bRequest is not
 supported.
 **/
int8_t class_req_cb(USB_device_request *req)
{
	int8_t status = USBD_ERR_NOT_SUPPORTED;

	return status;
}

/**
 @brief      USB standard request handler for GET_DESCRIPTOR
 @details    Handle standard GET_DESCRIPTOR requests from the host
 application.
 The hValue is parsed and the appropriate device,
 configuration or string descriptor is selected.
 For device and configuration descriptors the DFU
 state machine determines which descriptor to use.
 For string descriptors the lValue selects which
 string from the string_descriptors table to use.
 The string table is automatically traversed to find
 the correct string and the length is taken from the
 string descriptor.
 @param[in]        req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return                status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bmRequestType is not
 supported.
 **/
int8_t standard_req_get_descriptor(USB_device_request *req, uint8_t **buffer, uint16_t *len)
{
	uint8_t *src = NULL;
	uint16_t length = req->wLength;
	uint8_t hValue = req->wValue >> 8;
	uint8_t lValue = req->wValue & 0x00ff;
	uint16_t i;
	uint8_t slen;

	tfp_printf("Getting %d request\r\n", hValue);
	switch (hValue)
	{
	case USB_DESCRIPTOR_TYPE_DEVICE:
		src = (uint8_t *) &device_descriptor_keyboard;
		if (length > sizeof(USB_device_descriptor)) // too many bytes requested
			length = sizeof(USB_device_descriptor); // Entire structure.
		break;

	case USB_DESCRIPTOR_TYPE_CONFIGURATION:
		src = (uint8_t *) &config_descriptor_keyboard;
		if (length > sizeof(config_descriptor_keyboard)) // too many bytes requested
			length = sizeof(config_descriptor_keyboard); // Entire structure.
		break;

	case USB_DESCRIPTOR_TYPE_STRING:
		// Find the nth string in the string descriptor table
		i = 0;
		while ((slen = string_descriptor[i]) > 0)
		{
			src = (uint8_t *)&string_descriptor[i];
			if (lValue == 0)
			{
				break;
			}
			i += slen;
			lValue--;
		}
		if (lValue > 0)
		{
			// String not found
			return USBD_ERR_NOT_SUPPORTED;
		}
		// Update the length returned only if it is less than the requested
		// size
		if (length > slen)
		{
			length = slen;
		}
		break;

	case USB_DESCRIPTOR_TYPE_REPORT:
		src = (uint8_t *) &hid_report_descriptor;
		if (length > sizeof(hid_report_descriptor))
		{
			length = sizeof(hid_report_descriptor); // Entire structure.
		}
		break;

	default:
		tfp_printf("Unsupported request %d\r\n", hValue);
		return USBD_ERR_NOT_SUPPORTED;
	}

	*buffer = src;
	*len = length;

	return USBD_OK;
}

/**
 @brief      USB vendor request handler
 @details    Handle vendor requests from the host application.
 The bRequest value is parsed and the appropriate
 action or function is performed. Additional values
 from the USB_device_request structure are decoded
 and provided to other handlers.
 There are no vendor requests requiring handling in
 this firmware.
 @param[in]        req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return                status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bRequest is not
 supported.
 **/
int8_t vendor_req_cb(USB_device_request *req)
{
	int8_t status = USBD_ERR_NOT_SUPPORTED;

	return status;
}

/**
 @brief      USB suspend callback
 @details    Called when the USB bus enters the suspend state.
 @param[in]        status - unused.
 @return                N/A
 **/
void suspend_cb(uint8_t status)
{
	(void) status;
	return;
}

/**
 @brief      USB resume callback
 @details    Called when the USB bus enters the resume state
 prior to restating after a suspend.
 @param[in]  status - unused.
 @return     N/S
 **/
void resume_cb(uint8_t status)
{
	(void) status;
	return;
}

/**
 @brief      USB reset callback
 @details    Called when the USB bus enters the reset state.
 @param[in]        status - unused.
 @return                N/A
 **/
void reset_cb(uint8_t status)
{
	(void) status;

	USBD_set_state(USBD_STATE_DEFAULT);

	return;
}

/* For debugging endpoint transactions. */
void ep_cb(USBD_ENDPOINT_NUMBER ep_number)
{
	tfp_printf("EP%d\r\n", ep_number);
}


void gamepad(void)
{
	int8_t status;
	USBD_create_endpoint(USBD_EP_1, USBD_EP_INT, USBD_DIR_IN, USBD_EP_SIZE_8, USBD_DB_OFF, NULL /*ep_cb*/);

	JoystickState old_state;
	memset(&state, 0, sizeof(state));
	memset(&old_state, 0, sizeof(state));
	// Loop until disconnected.
	do {
		memcpy(&old_state, &state, sizeof(state));
		if (gpio_read(GPIO_FORWARD_PIN) == 0) {
			state.fb = -127;
		} else if (gpio_read(GPIO_BACK_PIN) == 0) {
			state.fb = 127;
		} else {
			state.fb = 0;
		}

		if (gpio_read(GPIO_LEFT_PIN) == 0) {
			state.lr = -127;
		} else if (gpio_read(GPIO_RIGHT_PIN) == 0) {
			state.lr = 127;
		} else {
			state.lr = 0;
		}
		state.buttons = gpio_read(GPIO_FIRE_PIN) == 0 | ((gpio_read(GPIO_FIRE2_PIN) == 0) << 1);

		if (USBD_get_state() == USBD_STATE_CONFIGURED) {
			if (!USBD_ep_buffer_full(USBD_EP_1)) {
				USBD_transfer(USBD_EP_1, (uint8_t *) &state, sizeof(state));
			}
		}
		// Update USBH status and perform callbacks if required.
		status = USBD_process();
	} while (status == USBD_OK);

	// Disconnected from host.
}

uint8_t usbd_testing(void)
{
	USBD_ctx usb_ctx;

	memset(&usb_ctx, 0, sizeof(usb_ctx));

	usb_ctx.standard_req_cb = NULL;
	usb_ctx.get_descriptor_cb = standard_req_get_descriptor;
	usb_ctx.class_req_cb = class_req_cb;
	usb_ctx.vendor_req_cb = vendor_req_cb;
	usb_ctx.suspend_cb = suspend_cb;
	usb_ctx.resume_cb = resume_cb;
	usb_ctx.reset_cb = reset_cb;
	usb_ctx.lpm_cb = NULL;
	usb_ctx.speed = USBD_SPEED_FULL;

	// Initialise the USB device with a control endpoint size
	// of 8 to 64 bytes. This must match the size for bMaxPacketSize0
	// defined in the device descriptor.
	usb_ctx.ep0_size = USB_CONTROL_EP_SIZE;
	//usb_ctx.ep0_cb = ep_cb;

	USBD_initialise(&usb_ctx);

	for (;;)
	{
		if (USBD_connect() == USBD_OK)
		{
			// Start the keyboard emulation code.
			gamepad();

			tfp_printf("Restarting\r\n");
		}
	}

	return 0;
}

void setup_pin(uint8_t num) {
    gpio_dir(num, pad_dir_input);
    gpio_pull(num, pad_pull_pullup);
	//gpio_slew(num, pad_slew_fast);
	// gpio_interrupt_enable(num, gpio_int_edge_raising);
}


void setup_gpio() {
	setup_pin(GPIO_BACK_PIN);
	setup_pin(GPIO_FORWARD_PIN);
	setup_pin(GPIO_LEFT_PIN);
	setup_pin(GPIO_RIGHT_PIN);
	setup_pin(GPIO_FIRE_PIN);
	setup_pin(GPIO_FIRE2_PIN);

	/* Attach an interrupt */
    // interrupt_attach(interrupt_gpio, (uint8_t)interrupt_gpio, ISR_gpio);
}

/* FUNCTIONS ***********************************************************************/

int main(void)
{
	/* Enable the UART Device... */
	sys_enable(sys_device_uart0);
	/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
	gpio_function(48, pad_uart0_txd); /* UART0 TXD */
	gpio_function(49, pad_uart0_rxd); /* UART0 RXD */

	uart_open(UART0,                    /* Device */
			1,                        /* Prescaler = 1 */
            UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
			uart_data_bits_8,         /* No. Data Bits */
			uart_parity_none,         /* Parity */
			uart_stop_bits_1);        /* No. Stop Bits */

	/* Print out a welcome message... */
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
	);

	/* Enable tfp_printf() functionality... */
	init_printf(UART0, tfp_putc);

	sys_enable(sys_device_timer_wdt);

	setup_gpio();
	
	interrupt_attach(interrupt_timers, (int8_t)interrupt_timers, ISR_timer);
	interrupt_enable_globally();



	/* Timer A = 1ms */
	timer_prescaler(1000);
	timer_init(timer_select_a, 100, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
	timer_enable_interrupt(timer_select_a);
	timer_start(timer_select_a);

	uint16_t prescaler = (uint16_t) 1000000;
	/* Timer B = 100ms */
	timer_prescaler(prescaler);
	timer_init(timer_select_b, 1000, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
	timer_enable_interrupt(timer_select_b);
	timer_start(timer_select_b);

	tfp_printf("(C) Copyright 2016, Future Technology Devices International Ltd. \r\n");
	tfp_printf("--------------------------------------------------------------------- \r\n");
	tfp_printf("Welcome to USBD HID Tester Example 1... \r\n");
	tfp_printf("\r\n");
	tfp_printf("Emulate a HID device connected to the USB Device Port\r\n");
	tfp_printf("--------------------------------------------------------------------- \r\n");

	uart_disable_interrupt(UART0, uart_interrupt_tx);
	uart_disable_interrupt(UART0, uart_interrupt_rx);

	usbd_testing();

	interrupt_detach(interrupt_timers);
	interrupt_disable_globally();

	sys_disable(sys_device_timer_wdt);

	// Wait forever...
	for (;;);

	return 0;
}
