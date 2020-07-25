#!/usr/bin/perl

# print STDERR "ARGV[0]: [$ARGV[0]]\n";

open(MF, "$ARGV[0]")
    	or die "Can't open \"$ARGV[0]\": $!";
    	
$ramstart = eval($ARGV[1]);    	
$ramsize = eval($ARGV[2]); 
$flashstart = eval($ARGV[3]); 
$flashsize = eval($ARGV[4]); 

$usedram = eval($ARGV[5]); 
$usedflash = eval($ARGV[6]); 

$ramend = $ramstart + $ramsize;
$flashend = $flashstart + $flashsize;

$code = 0;
$ram = 0;
$home = 0;

while(<MF>) 
{
	chomp;
	if (/bytes/) {
		if (/ABS/) {
		} else {
#			print "$_\n";		

			$n = 0;
			@A = split(' ');
		
#			while($n <= $#A) { print STDERR "A[$n]:=[$A[$n]]\t";	$n++;	} print STDERR "\n";
			
			$addr = eval("0x".$A[1]);
			$size = eval($A[4]."0");

			if (($ramstart <= $addr) && ($addr < $ramend)) {
				$ram += $size;
			} else {
				if (($flashstart <= $addr) && ($addr < $flashend)) {
					if (/HOME/) {
						if ($home == 0) {
							$code += $size;
							$home = 1;
						}
					} else {
						$code += $size;
					}
				} else {			
					if ($size > 0) {
						print STDERR sprintf("%d bytes outside address space: 0x%04X\n", $size, $addr);
					}
				}
			}

#			print STDERR sprintf("RAM: [%d], FLASH: [%d]   [%s]  addr:[%d]  size:[%d]\n", $ram, $code, $A[1]." ".$A[4], $addr, $size);	
		}
	}
}


# print STDERR sprintf("%c[1mRAM: %d, FLASH: %d  Delta: %d , %d%c[m\n", 27, $ram, $code, $ram - $usedram, $code - $usedflash, 27);	

print STDERR sprintf("RAM: %d, FLASH: %d  Delta: %d , %d\n", $ram, $code, $ram - $usedram, $code - $usedflash);	

print STDOUT sprintf("USED_RAM = %d\nUSED_FLASH = %d\n", $ram, $code);






