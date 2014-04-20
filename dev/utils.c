/* utilities file */


/* absolute value of a number
arg(0): the number
return: the absolute value of the number */
int abs(int number) {

	if (number < 0)
		return -number;
	else
		return number;
}

/* reduction of the absolute value of a number
arg(0): the number
arg(1): the value to be subtracted from the absolute value*/
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