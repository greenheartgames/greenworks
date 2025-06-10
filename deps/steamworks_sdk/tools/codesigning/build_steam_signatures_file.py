import os
import io
import sys
import re

# wrapper for print
def printw( message ):
    print( message )

try:
    import Crypto
    from Crypto.Signature import PKCS1_v1_5
    from Crypto.Hash import SHA
    from Crypto.PublicKey import RSA
except Exception as e:
    printw( "Missing required module: "+str(e) )
    sys.exit( -1 )
    
try:
    import zlib
except Exception as e:
    printw( "Missing required module: "+str(e) )
    sys.exit( -1 )

try:
    import hashlib
except Exception as e:
    printw( "Missing required module: "+str(e) )
    sys.exit( -1 )

try:
    import ctypes
except Exception as e:
    printw( "Missing required module: "+str(e) )
    sys.exit( -1 )


def usage():
    printw( "usage to verify a signaturefile:"+ sys.argv[0]+ " signaturefile publickeyfile" )
    printw( "usage to write a new signaturefile:"+ sys.argv[0]+ " signaturefile publickeyfile privatekeyfile newfilename" )
    printw( "" )

def readkeyfile( publickeyfilename ):
    # read the public key file.
    rawkey = open(publickeyfilename, mode='rb').read()
    # import as an RSA key
    key = RSA.importKey(rawkey)
    return key

def signmessage_add_digest( message, privatekeyfile ):
    key = RSA.importKey(open( privatekeyfile, mode='rb' ).read())
    h = SHA.new(message)
    signer = PKCS1_v1_5.new(key)
    signature = signer.sign(h)
    sighex = signature.encode("hex")
    return "DIGEST:"+sighex.upper()+"\r\n"

def checkdigest( signaturefilename, key ):
    with open(signaturefilename, mode='rb') as file:
        fileContent = file.read()

    # find the start of the digest
    idxtodigest = fileContent.find('DIGEST:')
    if idxtodigest == 0:
        return 0

    # the message is everything else
    message = fileContent[0:idxtodigest]

    digestpart = fileContent[idxtodigest+7:]
    digestpart = digestpart.replace("\r\n","")
    signature = digestpart.decode("hex")

    try:
        h = SHA.new(message)
        verifier = PKCS1_v1_5.new(key)
        if not verifier.verify(h, signature):
            return 0
    except Exception as e:
        printw( "could not verify signature: "+str(e) )
    return message

def crchex( crc32 ):
    crc32b = ctypes.c_uint32(crc32).value
    crc32hex = hex(crc32b).upper().replace("0X","").replace("L","")
    lenhex = len(crc32hex)
    # pad to 8 chars with leading 0s
    padding = 8 - lenhex
    while padding:
        crc32hex = "0"+crc32hex
        padding = padding - 1
    # byte swap
    crc32hex_reverse = crc32hex[6:8]+crc32hex[4:6]+crc32hex[2:4]+crc32hex[0:2]
    return crc32hex_reverse

def parsehashes( message, pathto ):
    # now verify all the file hashes
    newmessage = ""
    lines = message.split("\r\n")
    for line in lines:
        if len(line) == 0:
            break
        # parse the line, should be 5 parts
        # filename "~SHA1" sha1 ";CRC:" crc32
        parts=re.split(';|:|~',line)
        if len(parts) != 5:
            printw( "The file format is unexpected line:"+line )
            break
        if parts[1] != "SHA1" or parts[3] != "CRC":
            printw( "The file format is unexpected line:"+line )
            break

        hashprovided = parts[2]
        crcprovided = parts[4]
        onefile = parts[0]
        onefile = onefile.replace("...\\","")
        onefilepath = os.path.join( pathto, onefile )
        try:
            with open(onefilepath, mode='rb') as file:
                targetcontent = file.read()
        except:
            printw( "could not open: "+onefilepath )
            continue

        # compute sha1 of file
        mm = hashlib.sha1()
        mm.update(targetcontent)
        newhash = mm.hexdigest().upper()
        # compute crc32 of file
        crc32 = zlib.crc32(targetcontent)
        crc32hex = crchex(crc32)
        if ( newhash == hashprovided.upper() and
                crc32hex == crcprovided.upper() ):
            printw( "The hashes are correct for "+onefile )
        else:
            printw( "The hashes are different for "+onefile+" sha: "+newhash+" "+hashprovided.upper()+" CRC "+crc32hex+" "+crcprovided.upper() )
        # accumulate new hashes
        linenew = "...\\"+onefile+"~SHA1:"+newhash+";CRC:"+crc32hex+"\r\n"
        newmessage += linenew

    return newmessage

def signatures_need_update( signaturefilename, publickeyfilename ):

    if not os.path.exists( signaturefilename ):
        printw( "Signature file does not exist" )
        sys.stdout.flush()
        return False

    pathto = os.path.split(signaturefilename)[0]

    if not os.path.exists( publickeyfilename ):
        printw( "Public key file does not exist" )
        sys.stdout.flush()
        return False

    key = readkeyfile( publickeyfilename )

    message = checkdigest( signaturefilename, key )
    if len(message) == 0:
        printw( "failed to parse signature file " )
        return False

    newmessage = parsehashes( message, pathto )
    if len(newmessage) == 0:
        printw( "failed to parse hashes in signature data " )
        return False
    return newmessage != message


def write_new_signature_file( signaturefilename, publickeyfilename, privatekeyfilename, newsignaturefilename ):

    if not os.path.exists( signaturefilename ):
        printw( "Signature file does not exist" )
        sys.stdout.flush()
        return -1

    pathto = os.path.split(signaturefilename)[0]

    if not os.path.exists( publickeyfilename ):
        printw( "Public key file does not exist" )
        sys.stdout.flush()
        return -2

    if not os.path.exists( privatekeyfilename ):
        printw( "Private key file does not exist" )
        sys.stdout.flush()
        return -3

    if os.path.exists( newsignaturefilename ) and not os.access( newsignaturefilename, os.W_OK ):
        printw( "new signatures file not writeable " )
        sys.stdout.flush()
        return -4

    key = readkeyfile( publickeyfilename )

    message = checkdigest( signaturefilename, key )
    if len(message) == 0:
        printw( "failed to parse old signature file " )
        return -5

    newmessage = parsehashes( message, pathto )
    if len(newmessage) == 0:
        printw( "failed to create new signature data " )
        return -6

    with open( newsignaturefilename, mode='wb') as file:
        file.write( newmessage )
        hexdigest = signmessage_add_digest( newmessage, privatekeyfilename )
        file.write( hexdigest )

    printw( "new signatures file written successfully " )
    sys.stdout.flush()
    return 0


def main():

    if len( sys.argv ) != 5 and len( sys.argv ) != 3:
        usage()
        sys.exit(2)

    # common args
    signaturefilename = sys.argv[1]
    publickeyfilename = sys.argv[2]

    if len( sys.argv ) == 5:
        # only if writing a new file
        privatekeyfilename = sys.argv[3]
        newsignaturefilename = sys.argv[4]
        write_new_signature_file( signaturefilename, publickeyfilename, privatekeyfilename, newsignaturefilename )
    else:
        if signatures_need_update( signaturefilename, publickeyfilename ):
            printw( "Some signatures did not match" )
        else:
            printw( "All signatures matched" )


if __name__ == '__main__':
    main()

