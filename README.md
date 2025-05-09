# Guess-and-determine-Rebound
This repository contains the search and verify codes for the paper "Guess-and-Determine Rebound: Applications to Key Collisions on AES", which is accepted in Crypto2025.

Compile each C file with gcc, and run it:

aes128-2.c: search key collisions for 2-round aes128;

aes128-3.c: search key collisions for 3-round aes128;

aes192-5.c: search key collisions for 5-round aes192;

aes256-6.c: search key collisions for 6-round aes256;

aes192-7-semi-free.c: search key collisions for 7-round aes192 in free-target setting.

key_collision_verify.c: verify the key collisions results found by these codes.

we ran these codes on a desktop equipped with Intel Core i7-13700F @2.1 GHz and 16G RAM using one CPU core, and could find one key collision in a few seconds for 2-round AES-128, in a few hours for 3-round AES-128, and in a few minutes for 5-round AES-192, 6-round AES-256 without optimizing these codes. We find a semi-free-start collision on 7-round AES-192-DM in a few minutes.

