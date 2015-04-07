#!/usr/bin/perl
$green = 0;
$blue = 2047;
while ($red > 0) {
	$pid = fork();
	if (defined $pid && $pid == 0) {
	#	system("./client " . $red . " " . $green . " " . $blue);
		printf("$red, $green, $blue");
		exit 0;
	}
	$red--;
	$blue++;
}
while ($blue > 0) {
	$pid = fork();
	if (defined $pid && $pid == 0) {
		printf("$red, $green, $blue\n");
		exit 0;
	}
	$blue--;
	$green++;
}
while ($green > 0) {
	$pid = fork();
	if (defined $pid && $pid == 0) {
		printf("$red, $green, $blue\n");
		exit 0;
	}
	$green--;
	$red++;
}
