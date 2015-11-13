struct APP_to_TRANS {
	char APP_data;
	int APP_data_size;
	int DEST_address;

};

struct TRANS_to_NET {
	char Segment;
	int Segment_size;
};

//Function declarations:

to_NET_layer TRANS_send(from_NET_layer);
/* The "TRANS_send" function takes an input argument ("from_NET_layer") which is the APP data passed
	from the application layer. This input is a structure containing the three variables required for the
	transport layer processing.

	The return of the function is another struct (TRANS_receive_output) containing the processed segment
	and its size.*/

TRANS_receive_output TRANS_receive(TRANS_receive_input)


TRANS_send_output TRANS_send(TRANS_send_input){
	//	-take app data
	//	-check the total size
	//	-split into multiple segments if needed
	//	-add control bytes
	//	-add port bytes
	//	-add length byte
	//	-create a checksum
	//  -add the checksum bytes
	//  -final check that segment<121 bytes
	// 	-return the segment
	//  -wait for ACK or nACK
	//  -create and send next segment, or resend.
}



TO-DO: TRANS_receive(){
	//  -take an output
	//  -
}