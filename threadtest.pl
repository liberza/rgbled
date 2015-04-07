#!/usr/bin/perl
$red = 2047;
$green = 0;
$blue = 0;
$SIG{CHLD} = 'IGNORE';
while ($red > 0) {
	$pid = fork();
	if (defined $pid && $pid == 0) {
	#	system("./client " . $red . " " . $green . " " . $blue);
		printf("$red, $green, $blue\n");
		exit 0;
	}
	$red--;
	$green++;
}
while ($green > 0) {
	$pid = fork();
	if (defined $pid && $pid == 0) {
		printf("$red, $green, $blue\n");
		exit 0;
	}
	$green--;
	$blue++;
}
while ($green > 0) {
	$pid = fork();
	if (defined $pid && $pid == 0) {
		printf("$red, $green, $blue\n");
		exit 0;
	}
	$blue--;
	$red++;
}
