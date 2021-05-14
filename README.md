# HoneyPot_Password_Authentication

Implementation of Eugler's HoneyPot based Password Authentication System.

[Link to the Paper](https://ieeexplore.ieee.org/document/7047759)

File description.pdf contains the detail description of the project along with necessary screenshots of its working.

## Implementation Details

### A. mainserver.c and supportserver.c   

These programs are used for the main server processing which includes:
1. Implementing the login module and the new registration module.
2. During login process: The server checks if the entered password matches with that of any index for that user. If yes, then honeychecker server is contacted. If not, the situation is handled locally.
3. During registration process: The new user is assigned an index and construct a set of sweet indices randomly from the honeypot accounts and other users accounts. The entry is added to F1 file and all rows are shuffled randomly to avoid detection. F2 file is updated with the index number and the hash value. The correct index along with username is communicated with honeychecker for keeping a note.

Here, f2 and f1 are used for reading each entry for F2 and F1 file respectively. This array of structures is used for storing the contents.
1. md5hash iscomputed for each password. (Function defined in code)
2. TCP server is usedfor communicating with honeyserver.
3. supportserver.c is imported to mainserver.c for additional implementations and for making code understandability complex/ distributed (for preventing software reverse engineering based attacks)

### B. honeyserver.c
This program is the implementation of the honeyserver

1. Honeyserver recives usernum and matched index number from the mainserver. Honeyserver checks with correctindex.txt file if the matched index is indeed the sugarindex.
2. If matched index is not sugarindex, then it is either of a honeypot account or of a normal user account. For this, the index is checked with the files users.txt and honeypots.txt that contain the information of the indices of the users and honeypots of the system respectively.
3. If breach is detected, then this is noted down in the log file and reported to the admin.
4. Appropriate message is sent back to the mainserver notifying it of the login
status (successful or unsuccessful)
structures will store the correct index for each user. This will read from a protected file, which contains the details.
TCP Transfer Control Protocol is used for establishing the connections between the servers.
### C. updatehoney.c
1. Reads from files upusers.txt and uppasswds.txt to automatically update about 50 honeypot accounts and their respective
passwords in this authentication system.
2. Please note: I have already run this and hence now I have 77 accounts in the system. One can just change the username and password in these files and run this code. It will automoatically update the honeypots.
3. Please note: It is not required to run this file now, since, I have already run it.

### D. All supporting files
1. F1.txt – Contains username and corresponding sweet indices.
2. F2.txt – Contains index and corresponding hash value
3. honeyinfo/correctindex.txt – Contains details of correct index for each user (line matching done with F1.txt – not direct mapping)
4. honeyinfo/users.txt – Contains those indices who are normal users honeyinfo/honeypots.txt – Contains info of honeypot accounts

### E. All Log files mainserverlog.txt
Log file generated from mainserver.c supportserverlog.txt - Log file generated from supportserver.c honeyserverlog.txt - Log file generated from honeyserver.c
