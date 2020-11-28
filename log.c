
log(char* message, int severity)
{
    char severity_char = ' ';
    switch(severity)
    {
	case WARNING:
	     severity_char = '!';
             break;
	case ERROR:
	     severity_char = 'x';
             break;
	default:
	    severity_char = '*';
    }
	  

    printf("[%c] %s\n", severity_char, message);
}

