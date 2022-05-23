# Discord Teleporter 8mb

A basic program that disects a file into a set of 8mb chunks to send through Discord and bypass the very low limit.
The program also assimilates the chunks back into a file.

### Usage:
(it is best to add the file to environment variables for easy use)
Send:
`dt8mb send FILE.ext`
Receive:
`dt8mb receive FILE.ext`

Sending creates the chunks. receive assimilates the chunks under a target file with the extention `[ext]`. 
It is important that the `[FILE]` name in the call is identical to the received chunks minus the counter. You are free to set the extention.
example: chunk_001.dt8mb, `[File] == chunk`
