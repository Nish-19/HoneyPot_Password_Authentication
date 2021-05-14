main: mainserver.c supportserver.c
	gcc -Wall mainserver.c -o main -lcrypto -lssl
	echo "Compiled mainserver.c - run executable ./main"
honey: honeyserver.c 
	gcc -o honey honeyserver.c
	echo "Compiled honeyserver.c - run executable ./honey"
update: updatehoney.c
	gcc -Wall updatehoney.c -o uphoney -lcrypto -lssl
john_run: f2.txt rockyou.txt
	time john --wordlist=rockyou.txt --format=raw-md5 f2.txt
john_show: f2.txt
	john --show --format=raw-md5 f2.txt
get_wordlist:
	wget http://scrapmaker.com/data/wordlists/dictionaries/rockyou.txt
