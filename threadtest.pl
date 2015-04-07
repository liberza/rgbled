#!/usr/bin/perl
$pid = fork();
if (defined $pid && $pid == 0) {
	system("./client " . $red . " " . $green . " " . $blue);
	exit 0;
}
