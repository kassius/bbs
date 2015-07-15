void _event_handler(telnet_t *telnet, telnet_event_t *ev, void *user_data)
{
	struct user_t *user = (struct user_t*)user_data;
	int i=0;
	
	switch(ev->type)
	{
		case TELNET_EV_DATA:
			printf("TELNET_EV_DATA (%d bytes)\n", ev->data.size);
		if(ev->data.size<8) { printf("DATA IS: "); for(i=0;i<ev->data.size;i++) { printf("%d ", (unsigned char)ev->data.buffer[i]); } printf("\n"); }
		_process_key(telnet,user,ev->data.buffer,ev->data.size);
			break;

		case TELNET_EV_SEND:
			printf("TELNET_EV_SEND\n\t(%d bytes)\n", ev->data.size);
			_send(user->sock, ev->data.buffer, ev->data.size);
			break;
			
		case TELNET_EV_IAC:
			printf("TELNET_EV_IAC\n\t(iac: %d)", ev->iac.cmd);
			break;

		case TELNET_EV_WILL:
			printf("TELNET_EV_WILL\n");
			break;
			
		case TELNET_EV_DO:
			printf("TELNET_EV_WILL\n");
			break;

		case TELNET_EV_WONT:
			printf("TELNET_EV_WONT\n");
			break;
			
		case TELNET_EV_DONT:
			printf("TELNET_EV_DONT\n");
			break;
			
		case TELNET_EV_SUBNEGOTIATION:
			printf("TELNET_EV_SUBNEGOTIANTION (telopt is %d)\n", ev->sub.telopt);
			if(ev->sub.telopt == TELNET_TELOPT_NAWS)
			{
				user->width = (ev->sub.buffer[0] * 256) + (unsigned char)ev->sub.buffer[1];
				user->height = (ev->sub.buffer[2] * 256) + (unsigned char)ev->sub.buffer[3];

				//telnet_printf(telnet, "client width is x: %d and y: %d\n", user->width, user->height);
				printf("\tClient width is x: %d and y: %d\n", user->width, user->height);
				_draw(user->width, user->height, telnet, user);
			}
			break;
			
		case TELNET_EV_COMPRESS:
			printf("TELNET_EV_COMPRESS\n");
			break;
			
		case TELNET_EV_ZMP:
			printf("TELNET_EV_ZMP\n");
			break;
			
		case TELNET_EV_TTYPE:
			printf("TELNET_EV_TTYPE\n");
			break;
			
		case TELNET_EV_ENVIRON:
			printf("TELNET_EV_ENVIRON\n");
			break;
			
		case TELNET_EV_MSSP:
			printf("TELNET_EV_MSSP\n");
			break;
			
		case TELNET_EV_WARNING:
			printf("TELNET_EV_WARNING\n");
			break;
			
		case TELNET_EV_ERROR:
			close(user->sock);
			user->sock = -1;
			telnet_free(user->telnet);
			printf("TELNET_EV_ERROR\n");
			break;

		default:
			break;
	}
}
