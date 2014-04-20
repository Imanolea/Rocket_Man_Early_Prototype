/* utilities file */


// returns the absolute value of a number
int abs(int number) {

	if (number < 0)
		return -number;
	else
		return number;
}

void reduceabs(int *number, int value) {
	int ret = *number;

	if (ret < 0) {
		ret += value;
		if (ret == -256) // compiler's fault
			
			(*number) = 0;
		else
			(*number) = ret;		
	} else if (ret > 0) {
		ret -= value;
		if (ret <= 0)
			(*number) = 0;
		else
			(*number) = ret;
	} else
		(*number) = 0;
}