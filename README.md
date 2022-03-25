# One-Time-Pads
Client/Server One-Time Pads application C program

How to run the program:
compileall all five files: 
./compileall  

Generate long enough encryption key :/n 
./keygen 70000 > key70000

Run the daemons. "encport"/"decport" can be the 50000+ prot such as 54321 and 54322
./enc_server $encport &
./dec_server $decport &

Send example text to the enc_server, salve the encrypted text to the ciphertext1 file:
./enc_client plaintext1 key70000 $encport > ciphertext1

Send encrypted text to the dec_server, salve the decrypted text to the plaintext1_a file:
./dec_client ciphertext1 key70000 $decport > plaintext1_a
