#DEPRECATED DEPRECATEDDEPRECATED DEPRECATED  

#oak_sign

##oak_sign v1 usage: oak_sign (filename) [optional: system or unsign]

signs "filename", an oak compatible bin file, for OTA usage
if system is specified, it will be signed as a system
update, if unsign is specified any signature will be
removed, otherwise it is signed as a user program


###Padding structure:

Appends padding to input file to bring it to a size divisible by 512. If that padding is not at least 16 bytes, file is extended with an additional 512 bytes of padding. (IF filesize%512 < 16 THEN 512+filesize%512 ELSE filesize%512)

The last 16 bytes of padding are as follows:

- 0-3: 0xFF
- 4-7: Unsigned 4-byte integer of the original file size
- 8: Flag byte (see below)
- 9: 0x4F ("O")
- 10: 0x61 ("a")
- 11: 0x6B ("k")
- 12-15: 0xFF

Flag Byte: 

The flag byte denotes what type of rom the bin file contains.

- 0x00: User Rom
- 0x01: System Rom

###Input file requirements:

oak_sign will apply this to any input file that is non-empty. To work with the Oak platform the bin file must be compiled from the Oak Arduino core, linked with the Oak linker script, and packaged using esptool2 with the boot2 and irom options.
