# GraceCoin

Made in c++

By: Harsh Raj

Email: harsh.raj.cd.ece21@itbhu.ac.in

Command List:

         "help: show basic commands with descriptions",
         " -help: for command description, put after another command",
         "help-all: show all commands with description",
         "show w: show warranty information about the licence",
         "show c: show copying information about the licence",
         "create-wa: generate new wallet address",
         "buy: buy an amount, must specify amount after typing buy",
         "send: send an amount to another wallet",
         "sell: same as send but send to non-existant wallet address",
         "e-wallet-aes128: encrypt wallet with aes256, do not provide wallet address here, provide key",
         "e-wallet-aes192: encrypt wallet with aes192, do not provide wallet address here, provide key",
         "e-wallet-aes256: encrypt wallet with aes256, do not provide wallet address here, provide key",
         "e-wallet-aes128-genkey: encrypt wallet with aes256, do not provide wallet" +
         std::string("address here, do not provide key, for command line, add space after aes128"),
         "e-wallet-aes192-genkey: encrypt wallet with aes192, do not provide wallet" +
         std::string(" address here, do not provide key, for command line, add space after aes192"),
         "e-wallet-aes256-genkey: encrypt wallet with aes256, do not provide wallet" +
         std::string(" address here, do not provide key, for command line, add space after aes256"),
         "decrypt-wallet: decrypt wallet using chosen encryption algorithm, provide key",
         "get p-w keys: request private wallet keys",
         "get p-trns-data: request single transaction data, provide transaction index in wallet",
         "del-wallet: delete your wallet address, make sure wallet is empty before" +
         std::string(" doing so, wallet components will be deleted and cannot be brought back"),
         "exit: will terminate and exit program",
         "quit: will terminate and exit program",
         "burn [amount]: burn an amount of crypto(send to dead wallet address), provide amount",
         "hash-sha512 [input]: hash input with sha512",
         "enc-aes128 -genkey [input,key]: encrypt input with aes128, key is generated for you",
         "enc-aes192 -genkey [input,key]: encrypt input with aes192, key is generated for you",
         "enc-aes256 -genkey [input,key]: encrypt input with aes256, key is generated for you",
         "enc-aes128 [input,key]: encrypt input with aes128, use own key in hex format",
         "enc-aes192 [input,key]: encrypt input with aes192, use own key in hex format",
         "enc-aes256 [input,key]: encrypt input with aes256, use own key in hex format",
         "dec-aes128 [input,key]: decrypt ciphertext with aes128, provide key",
         "dec-aes192 [input,key]: decrypt ciphertext with aes192, provide key",
         "dec-aes256 [input,key]: decrypt ciphertext with aes256, provide key",
         "get myahr: print my average hashrate",
         "get blockchain: prints all blocks in blockchain",
         "get block-hash [block index]: get block hash, provide index",
         "get block-nonce [block index]: get block nonce, provide index",
         "get block-timestamp [block index]: get block timestamp, provide index",
         "get block-merkle-r [block index]: get merkle root of block, provide index",
         "get block-difficulty [block index]: get difficulty of block, provide index",
         "get block-ahr [block index]: get average hash rate of block miners, provide index",
         "get nblocktime: get next block generation time",
         "get blockchain-size: print amounts of blocks in blockchain",
         "get version: get blockchain version",
         "get mempool: print verified mempool hashes in current block",
         "enc-algs: available encryption/decryption algorithms",
         "start mine: start mining",
         "dump-wallet512: dump 512-bit wallet address as hex", // after this is not in version 1
         "get tr-target: print transaction target",
         "get tr-hash: print transaction hash",
         "get tr-ciphertext [trns index]: print transaction ciphertext",
         "get tr-timestamp [trns index]: print transaction timestamp",
         "dump all-trnsData: dump all transaction data in wallet",
         "get blockchain-ahr: get average hashrate over all blockchain",
         "get block-target [block index]: get block target hash, provide index"
