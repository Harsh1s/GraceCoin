#ifndef CONSOLE_UI_H_
#define CONSOLE_UI_H_

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <random>
#include <time.h>
#include <tuple>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define OS_WINDOWS 1
#else
#include <unistd.h>
#endif

/* for UI */
struct userData
{
    std::map<std::shared_ptr<uint64_t>,
             std::vector<std::shared_ptr<uint8_t>>>
        walletMap;
    std::map<std::string, std::shared_ptr<uint8_t>> &transactions;
    std::vector<std::shared_ptr<uint64_t>> &transactionhashesW;
    std::vector<uint32_t> &trnsLengths;

    int32_t setBalance()
    {
        std::string plaintext;
        AES::AES256 aes256;
        int32_t storedCrypto = 0; // set to zero and recalculate
        for (auto const [ciphertext, b32key] : transactions)
        {
            plaintext = aes256.decrypt(ciphertext, b32key);
            std::string str_amount = "";
            size_t index = plaintext.find("amount: ");
            int lenIndex;

            // delete padding caused by encryption
            // check which length creates correct hash
            for (uint32_t c = 0; c < trnsLengths.size(); c++)
            {
                plaintext.erase(trnsLengths[c], plaintext.length() - trnsLengths[c]);
                std::shared_ptr<uint64_t> hash = sha512(plaintext);
                for (int i = 0; i < transactionhashesW.size(); i++)
                {
                    for (int j = 0; j < 8; j++)
                        if (transactionhashesW[i].get()[j] == hash.get()[j])
                        {
                            lenIndex = c;
                            goto stop;
                        }
                }
            stop:
                for (int k = lenIndex; k < plaintext.length(); k++)
                {
                    str_amount += plaintext[k];
                }
                // calculate wallet balance
                int32_t amount = static_cast<int32_t>(std::stoul(str_amount));
                if (plaintext.starts_with("buys"))
                {
                    storedCrypto += amount;
                }
                else if (plaintext.starts_with("send"))
                {
                    storedCrypto -= amount;
                }
            }
        }
        return storedCrypto;
    }
};

// user interface package
namespace ui
{
    /* licence information commands for terminal interaction */
    // show w: warranty
    inline void show_w_command()
    {
        std::cout << "THIS IS A HOBBY PROJECT MEANT FOR UNDERSTANDING THE CONCEPT OF CRYPTOCURRENCY AND BLOCKCHAIN.\nHENCE, EXPECT NO WARRANTY.\n";
    }

    // show c: copying
    inline void show_c_command()
    {
        std::cout << "\nThis Project comes under MIT Opensource license. You are free to use, modify and learn from it.";
    }
    // find if index is digit
    inline uint64_t check_index_block(char **argv, int argc)
    {
        uint64_t index;
        std::string str_difficulty;
        std::string index_str;
        std::stringstream ss;

        if (argc != 4)
        {
            std::cout << "\ninput index of block (index starts from zero):\t";
            std::cin >> index_str;
            for (int c = 0; c < index_str.length(); c++)
            {
                if (!isdigit(index_str[c]))
                {
                    exit(EXIT_FAILURE);
                }
            }
            ss << index_str;
            ss >> index;
        }
        else
        {
            ss << argv[3];
            ss >> index_str;
            for (int c = 0; c < index_str.length(); c++)
            {
                if (!isdigit(argv[3][c]))
                {
                    exit(EXIT_FAILURE);
                }
            }
            ss.clear();
            ss << argv[3];
            ss >> index;
        }
        return index;
    }

    // internal package for anonymous functions
    namespace
    {
        inline std::pair<uint64_t, bool> get_block_index_ui(std::string userInput)
        {
            uint64_t block_index;
            std::string block_index_str;
            std::stringstream ss_block;
            bool NaN = false;
            if (userInput.length() >= 16)
            {
                block_index_str = userInput.substr(15, userInput.length() - 1);
                for (int c = 0; c < block_index_str.length(); c++)
                {
                    if (!isdigit(block_index_str[c]))
                    {
                        std::cout << "NaN\n";
                        NaN = true;
                        break;
                    }
                }
                if (!NaN)
                {
                    ss_block << block_index_str;
                    ss_block >> block_index;
                    ss_block.clear();
                }
            }
            else
            {
                std::cout << "\ninput index of block (index starts from zero):\t";
                std::cin >> block_index_str;
                for (int c = 0; c < block_index_str.length(); c++)
                {
                    if (!isdigit(block_index_str[c]))
                    {
                        NaN = true;
                        break;
                    }
                }
                if (!NaN)
                {
                    ss_block << block_index_str;
                    ss_block >> block_index;
                    ss_block.clear();
                }
                else
                {
                    std::cout << "\nNaN\n";
                }
            }
            return {block_index, NaN};
        }
    } /* internal namespace */

    inline void consoleUI(int argc, std::vector<std::string> commandDescriptions,
                          std::string blockchain_version, std::shared_ptr<uint64_t> &walletAddress, std::vector<std::shared_ptr<uint64_t>> &walletAddresses, std::map<std::shared_ptr<uint64_t>, std::vector<std::shared_ptr<uint8_t>>> &walletMap,
                          std::vector<std::shared_ptr<uint8_t>> &userAESmapkeys,
                          int32_t &storedCrypto, std::shared_ptr<uint64_t> &secondWallet, std::vector<std::shared_ptr<uint64_t>> &transactionhashesW, std::vector<uint32_t> &trnsLengths,
                          std::vector<std::shared_ptr<uint64_t>> &mempool,
                          std::string &ciphertextW, std::string &ciphertextK1,
                          std::string &ciphertextK2, std::string &usedEncAlg,
                          std::map<std::string, std::shared_ptr<uint8_t>>
                              transactions,
                          std::vector<std::shared_ptr<uint8_t>>
                              AESkeysTr,
                          std::set<std::string> &blockchain,
                          std::vector<uint32_t> &all_trns_lengths, std::map<std::string, std::shared_ptr<uint8_t>> &transactions_enc,
                          std::vector<std::shared_ptr<uint64_t>> &blockhashes,
                          bool &block_mined, std::vector<std::shared_ptr<uint64_t>> &unsafe_mempool, std::shared_ptr<uint64_t> &merkle_root)
    {
        WalletAddress wallet_address = WalletAddress();
        SHA512 hash = SHA512();
        Block block = Block();
        PoW ProofofWork = PoW();
        AES::AES128 aes128;
        AES::AES192 aes192;
        AES::AES256 aes256;

        std::string userInput;
        std::map<std::shared_ptr<uint64_t>, std::vector<std::shared_ptr<uint8_t>>>::
            iterator itWalletMap = walletMap.begin();
        std::map<std::string, std::shared_ptr<uint8_t>>::iterator
            it_trns_enc = transactions_enc.begin();
        bool terminate = false;

        if (argc >= 2)
        {
            console_ui_activate = true;
        }

        // only activated if global value in main.cpp is true
        while (console_ui_activate)
        {
            while (!terminate)
            {
                std::cout << "input:\t";
                std::getline(std::cin, userInput);
                if (userInput == "help")
                {
                    for (int c = 0; c < 18; c++)
                        std::cout << commandDescriptions[c] << "\n";
                    break;
                }
                else if (userInput == "help-all")
                {
                    if (blockchain_version != "1.0")
                    {
                        for (int c = 0; c < commandDescriptions.size(); c++)
                            std::cout << commandDescriptions[c] << "\n";
                    }
                    else
                    {
                        for (int c = 0; c < commandDescriptions.size() - 9; c++)
                            std::cout << commandDescriptions[c] << "\n";
                    }
                    break;
                }
                else if (userInput.length() > 6 && userInput.substr(userInput.length() - 6,
                                                                    userInput.length()) == " -help")
                {
                    bool commandExists = false;
                    for (int c = 0; c < commandDescriptions.size() - 1; c++)
                    {
                        if (commandDescriptions[c].starts_with(userInput.substr(0, userInput.length() - 6)))
                        {
                            std::cout << "\n"
                                      << commandDescriptions[c];
                            commandExists = true;
                        }
                    }
                    if (!commandExists)
                    {
                        std::cout << "command doesn\'t exist";
                    }
                    break;
                }
                else if (userInput == "create-wa")
                {
                    std::cout << "\ncreating wallet address...\n";
                    auto [fstNewAddrs, sndNewAddrs] = wallet_address.GenerateNewWalletAddress("dump aes256-key");
                    std::cout << "wallet address created\nwallet address:\t";
                    walletAddress = fstNewAddrs;
                    std::cout << std::hex << to8_64_str(walletAddress);
                    std::cout << std::endl
                              << "save these values on your device\n";
                    walletAddresses.push_back(walletAddress);
                    walletMap.insert(itWalletMap, std::pair<std::shared_ptr<uint64_t>,
                                                            std::vector<std::shared_ptr<uint8_t>>>(walletAddress,
                                                                                                   sndNewAddrs));
                    std::cout << "wallet address saved on map\n";
                    break;
                }
                else if (userInput == "buy" || userInput == "send" || userInput == "sell")
                {
                    uint32_t amount;
                    std::string secondWalletAd;

                    // ask for walletAddress of receiver or sender, key isn't requiried
                    if (userInput == "buy")
                    {
                        secondWalletAd = "sender";
                    }
                    else
                    { // send or sell
                        secondWalletAd = "receiver";
                    }
                    if (walletMap.empty())
                    {
                        std::cout << "wallet map is empty, input your wallet address."
                                  << " If you don\'t have one, type \"nw \" here,press enter, "
                                  << "if you have one, press enter, copy paste wallet address"
                                  << " from where you saved it:\t";
                        std::string noWallet;
                        std::cin >> noWallet;
                        std::string strWallet;
                        if (noWallet == "yw")
                        {
                            std::cin >> strWallet;
                            walletAddress = usrInWallet512(strWallet);
                            // verify inputted wallet
                            wallet_address.verifyInputWallet(walletAddresses, walletAddress);

                            // if walletAddress valid, input wallet keys
                            std::cout << "\ninput your aes256 wallet key 1 as hex:\t";
                            std::string key1Str;
                            std::cin >> key1Str;
                            userAESmapkeys[0] = aesKeyToSPtr<uint8_t>(key1Str);
                            std::cout << "\ninput your aes256 wallet key 2 as hex:\t";
                            std::string key2Str;
                            std::cin >> key2Str;
                            userAESmapkeys[1] = aesKeyToSPtr<uint8_t>(key2Str);
                            walletMap.insert(itWalletMap, std::pair<std::shared_ptr<uint64_t>,
                                                                    std::vector<std::shared_ptr<uint8_t>>>(walletAddress, userAESmapkeys));
                        }
                        else
                        { // only difference is first trWallet parameter is nullptr
                            storedCrypto = 0;
                            walletAddress = nullptr;
                        }
                    }
                    else
                    { // if walletMap not empty
                        std::cout << "\nwallet address found\n";
                    }
                    std::cout << "\ninput " << secondWalletAd << "s wallet address:\t";
                    std::string str_wallet;
                    std::cin >> str_wallet;
                    secondWallet = usrInWallet512(str_wallet);
                    wallet_address.verifyInputWallet(walletAddresses, walletAddress);
                    std::cout << "\nwallet data verified\n";
                    struct Wallet trWallet
                    {
                        walletAddress, userAESmapkeys, walletMap
                    };
                    std::cout << "\ninput amount:\t";
                    std::cin >> amount;

                    struct userData user_data
                    {
                        walletMap, transactions, transactionhashesW,
                            trnsLengths
                    };
                    storedCrypto = user_data.setBalance();

                    /* since newTransaction doesn't have sell in sellorbuy and both
                     * perform the same task for now
                     */
                    if (userInput == "sell")
                    {
                        userInput = "send";
                    }
                    auto [newWA, newKeys] = trWallet.new_transaction(secondWallet,
                                                                     walletAddress,
                                                                     amount, mempool,
                                                                     userInput,
                                                                     transactionhashesW,
                                                                     transactions,
                                                                     storedCrypto,
                                                                     trnsLengths,
                                                                     all_trns_lengths,
                                                                     "dump aes256-key");

                    // append transactions of single wallet to all transactions
                    // ciphertexts in blockchain
                    for (const auto [ciphertxt, trns_keys] : transactions)
                    {
                        // add transaction ciphertext and keys if its not already in
                        // transactions_enc
                        if (transactions_enc.find(ciphertxt) == transactions_enc.end())
                        {
                            transactions_enc.insert(it_trns_enc, std::pair<std::string,
                                                                           std::shared_ptr<uint8_t>>(ciphertxt,
                                                                                                     trns_keys));
                        }
                    }

                    walletAddress = newWA;
                    userAESmapkeys = newKeys;
                    break;
                }
                else if (userInput == "e-wallet-aes128" || userInput == "e-wallet-aes192" ||
                         userInput == "e-wallet-aes256" || userInput == "e-wallet-aes128-genkey" ||
                         userInput == "e-wallet-aes192-genkey" || userInput == "e-wallet-aes256-genkey")
                {
                    std::string usrCommand;
                    std::string ACmndNoKey;
                    std::string ACmndWithKey; // alternative command with key
                    std::string algorithm;
                    uint32_t keysize;
                    usrCommand = userInput;
                    std::string aesAlgKey;
                    if (userInput == "e-wallet-aes128" || userInput == "e-wallet-aes128-genkey")
                    {
                        ACmndNoKey = "enc-aes128";
                        ACmndWithKey = "enc-aes128-genkey";
                        algorithm = "aes128";
                        keysize = 16;
                    }
                    else if (userInput == "e-wallet-aes192" || userInput == "e-wallet-aes192-genkey")
                    {
                        ACmndNoKey = "enc-aes192";
                        ACmndWithKey = "enc-aes192-genkey";
                        algorithm = "aes192";
                        keysize = 24;
                    }
                    else if (userInput == "e-wallet-aes256" || userInput == "e-wallet-aes256-genkey")
                    {
                        ACmndNoKey = "enc-aes256";
                        ACmndWithKey = "enc-aes256-genkey";
                        algorithm = "aes256";
                        keysize = 32;
                    }
                    std::shared_ptr<uint8_t> encWalletAesAlgKey(new uint8_t[keysize]);
                    if (walletMap.empty())
                    {
                        std::cout << "no wallet saved, if you want to encrypt manually, try "
                                  << "\"" << ACmndNoKey << "\" and input both input and key, if you"
                                  << " want to use an automatically generated key, use"
                                  << " \"" << ACmndWithKey << "\".";
                    }
                    else
                    {
                        if (userInput != "e-wallet-aes128-genkey" || userInput != "e-wallet-aes192-genkey" || userInput != "e-wallet-aes256-genkey")
                        {
                            std::cout << "\nwallet found\ninput " << algorithm << " key as hex"
                                      << "(hex digits only):\t";
                            std::cin >> aesAlgKey;
                            encWalletAesAlgKey = aesKeyToSPtr<uint8_t>(aesAlgKey, keysize);
                            std::cout << "\nis the key you inputted correct as hex integer:\t";
                            for (int c = 0; c < keysize; c++)
                            {
                                std::cout << std::hex << (short)encWalletAesAlgKey.get()[c];
                            }
                            std::cout << "\n\ninteger value will have a few missing zeros"
                                      << "which is fine but if a big part or everything is"
                                      << "wrong, that is a problem, please stop the process and "
                                      << "report the problem";
                            std::cout << std::endl;
                        }
                        else
                        {
                            if (userInput == "e-wallet-aes128-genkey")
                            {
                                encWalletAesAlgKey = generateAES128Key();
                            }
                            else if (userInput == "e-wallet-aes192-genkey")
                            {
                                encWalletAesAlgKey = generateAES192Key();
                            }
                            else
                            {
                                encWalletAesAlgKey = generateAES256Key();
                            }
                        }
                        std::vector<std::shared_ptr<uint8_t>> walletKeys;
                        for (const auto [wa, walletkeys] : walletMap)
                        {
                            walletAddress = wa;
                            walletKeys = walletkeys;
                        }
                        if (algorithm == "aes128")
                        {
                            ciphertextW = aes128.encrypt(to8_64_str(walletAddress),
                                                         encWalletAesAlgKey);
                            ciphertextK1 = aes128.encrypt(aesKeyToStr<uint8_t>(walletKeys[0], 16),
                                                          encWalletAesAlgKey);
                            ciphertextK2 = aes128.encrypt(aesKeyToStr<uint8_t>(walletKeys[1], 16),
                                                          encWalletAesAlgKey);
                        }
                        else if (algorithm == "aes192")
                        {
                            ciphertextW = aes192.encrypt(to8_64_str(walletAddress),
                                                         encWalletAesAlgKey);
                            ciphertextK1 = aes192.encrypt(aesKeyToStr<uint8_t>(walletKeys[0], 24),
                                                          encWalletAesAlgKey);
                            ciphertextK2 = aes192.encrypt(aesKeyToStr<uint8_t>(walletKeys[1], 24),
                                                          encWalletAesAlgKey);
                        }
                        else
                        {
                            ciphertextW = aes256.encrypt(to8_64_str(walletAddress),
                                                         encWalletAesAlgKey);
                            ciphertextK1 = aes256.encrypt(aesKeyToStr<uint8_t>(walletKeys[0]),
                                                          encWalletAesAlgKey); // keysize = 32
                            ciphertextK2 = aes256.encrypt(aesKeyToStr<uint8_t>(walletKeys[1]),
                                                          encWalletAesAlgKey); // keysize = 32
                        }
                        std::cout << "\nciphertext of wallet address:\t" << ciphertextW
                                  << "\n\nwarning: an "
                                  << "encrypted wallet address is not"
                                  << " usable until you decrypt it";
                        walletAddress = nullptr;
                        std::cout << "\n\nciphertext of key 1:\t" << ciphertextK1;
                        std::cout << "\n\nciphertext of key 2:\t" << ciphertextK2;
                        std::cout << "\n\nsave these values and keys as they won\'t be "
                                  << "saved here and you won\'t be able to access your wallet again";
                        std::cout << "\nunencrypted wallet data will be gone until you decrypt it,"
                                  << " are you sure you want to continue\ntype \"y\" for yes, "
                                  << "\"n\" for no";
                        std::string confirm;
                        bool terminateEWallet = false;
                        while (!terminateEWallet)
                        {
                            std::cin >> confirm;
                            if (confirm == "n" or confirm == "no")
                            {
                                std::cout << "\nprocess terminated, wallet not encrypted.";
                                // reset ciphertexts
                                ciphertextW = "";
                                ciphertextK1 = "";
                                ciphertextK2 = "";
                                terminateEWallet = true;
                            }
                            else if (confirm == "y" or confirm == "yes")
                            {
                                std::cout << "\nclearing unencrypted wallet data...";
                                usedEncAlg = algorithm;
                                walletMap.clear();
                                std::cout << "\ncomplete\nencryption key:\t" << std::hex
                                          << aesKeyToStr<uint8_t>(encWalletAesAlgKey, keysize);
                                terminateEWallet = true;
                            }
                            else
                            {
                                std::cout << "invalid input\n(y) or (n)";
                            }
                        }
                    }
                    break;
                }
                else if (userInput == "decrypt-wallet")
                {
                    uint32_t keysize;
                    std::string aesKeyStr;
                    if (usedEncAlg == "aes128")
                    {
                        keysize = 16;
                    }
                    else if (usedEncAlg == "aes192")
                    {
                        keysize = 24;
                    }
                    else if (usedEncAlg == "aes256")
                    {
                        keysize = 32;
                    }
                    std::shared_ptr<uint8_t> edkey(new uint8_t[keysize]);
                    std::string decStrWalletaddress;
                    std::string decStrWalletk1;
                    std::string decStrWalletk2;
                    std::shared_ptr<uint8_t> decWalletk1(new uint8_t[keysize]);
                    std::shared_ptr<uint8_t> decWalletk2(new uint8_t[keysize]);
                    if (walletMap.empty() && ciphertextW != "")
                    {
                        std::cout << "ciphertext of wallet address found and no unencrypted data."
                                  << "Decrypting a wallet address using the " << usedEncAlg
                                  << ".\nciphertext of wallet address:\t" << ciphertextW
                                  << "\n\nciphertext of first " << usedEncAlg << " key:\t"
                                  << ciphertextK1 << "\n\nciphertext of second " << usedEncAlg
                                  << " key:\t" << ciphertextK2 << "\n\ninput " << keysize
                                  << " byte " << usedEncAlg << " key:\t";
                        std::cin >> aesKeyStr;
                        edkey = aesKeyToSPtr<uint8_t>(aesKeyStr, keysize);
                        std::cout << "\ndecrypting wallet data...\n";
                        if (usedEncAlg == "aes128")
                        {
                            decStrWalletaddress = aes128.decrypt(ciphertextW, edkey);
                            decStrWalletk1 = aes128.decrypt(ciphertextK1, edkey);
                            decStrWalletk2 = aes128.decrypt(ciphertextK2, edkey);
                        }
                        else if (usedEncAlg == "aes192")
                        {
                            decStrWalletaddress = aes192.decrypt(ciphertextW, edkey);
                            decStrWalletk1 = aes192.decrypt(ciphertextK1, edkey);
                            decStrWalletk2 = aes192.decrypt(ciphertextK2, edkey);
                        }
                        else if (usedEncAlg == "aes256")
                        {
                            decStrWalletaddress = aes256.decrypt(ciphertextW, edkey);
                            decStrWalletk1 = aes256.decrypt(ciphertextK1, edkey);
                            decStrWalletk2 = aes256.decrypt(ciphertextK2, edkey);
                        }
                        /* delete padding caused by aes encryption, since all keys and
                         * wallet address length is 512-bits as string which is a multiple
                         * of aes block(16 bytes), its not necesarry to delete padding
                         *  but is a good precation to take just in case
                         */
                        uint32_t keySizeBits = keysize << 3;
                        decStrWalletaddress.erase(512, 512 - decStrWalletaddress.length());
                        decStrWalletk1.erase(keySizeBits, keySizeBits - decStrWalletk1.length());
                        decStrWalletk2.erase(keySizeBits, keySizeBits - decStrWalletk2.length());
                        walletAddress = usrInWallet512(decStrWalletaddress);
                        decWalletk1 = aesKeyToSPtr<uint8_t>(decStrWalletk1, keysize);
                        decWalletk2 = aesKeyToSPtr<uint8_t>(decStrWalletk2, keysize);
                        std::cout << "wallet components decrypted. Wallet address as plaintext:\t"
                                  << decStrWalletaddress << "\ndecrypted first " << usedEncAlg
                                  << " key as plaintext:\t" << decStrWalletk1
                                  << "\ndecrypted second " << usedEncAlg
                                  << " key as plaintext:\t" << decStrWalletk2;
                        std::vector<std::shared_ptr<uint8_t>> walletKeysDec;
                        walletKeysDec.push_back(decWalletk1);
                        walletKeysDec.push_back(decWalletk2);
                        walletMap.insert(itWalletMap, std::pair<std::shared_ptr<uint64_t>, std::vector<std::shared_ptr<uint8_t>>>(walletAddress, walletKeysDec));
                        std::cout << "\nwallet data saved\n";
                    }
                    else
                    {
                        std::cout << "\nNO ENCRYPTED WALLET FOUND\n";
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                // delete command or create a way to verify user
                else if (userInput == "get p-w keys")
                {
                    std::cout << "\nsearching wallet for keys...\n";
                    if (walletMap.empty())
                    {
                        std::cout << "error: no wallet found";
                        exit(EXIT_FAILURE);
                    }
                    for (const auto [walletAd, walletKeys] : walletMap)
                    {
                        std::cout << "key 1:\t";
                        for (int c = 0; c < 32; c++)
                            std::cout << std::hex
                                      << (short)walletKeys[0].get()[c];

                        std::cout << std::endl
                                  << std::endl
                                  << "key 2:\t";
                        for (int c = 0; c < 32; c++)
                            std::cout << std::hex
                                      << (short)walletKeys[1].get()[c];
                    }
                    std::cout << "\n\nif you want to also see your wallet address, type \""
                              << "get wa\"";
                    break;
                }
                else if (userInput == "get p-trns-data")
                {
                    if (transactionhashesW.empty())
                    {
                        std::cout << "\nzero transactions in wallet\n";
                        exit(EXIT_FAILURE);
                    }
                    if (walletMap.empty())
                    {
                        std::cout << "\nno wallet found\n";
                        exit(EXIT_FAILURE);
                    }
                    std::cout << "\nthere are " << transactionhashesW.size()
                              << " transactions in your wallet\nstate index of transaction"
                              << " (index starts from zero), if you want all transaction "
                              << "data or you don\'t know the index, type\"get all-trns-data\":\t";
                    uint64_t index;
                    std::cin >> index;
                    std::cout << "\ntransaction hash:\t";
                    for (int c = 0; c < 8; c++)
                        std::cout << std::hex
                                  << transactionhashesW[index].get()[c];
                    std::string plaintext;
                    uint64_t trnsIndex;
                    std::string ciphertextTr;
                    std::shared_ptr<uint8_t> trnsKey;
                    std::string correctPlaintext;
                    // find ciphertext and key index of transaction encryption map
                    for (const auto [cph, ckey] : transactions)
                    {
                        /* delete padding caused by encryption
                           check which length creates correct hash to find index using
                           single wallet mempool */
                        plaintext = aes256.decrypt(cph, ckey);
                        for (uint64_t c = 0; c < trnsLengths.size(); c++)
                        {
                            plaintext.erase(trnsLengths[c], plaintext.length() - trnsLengths[c]);
                            std::shared_ptr<uint64_t> hash = sha512(plaintext);
                            for (uint64_t i = 0; i < transactionhashesW.size(); i++)
                            {
                                for (int j = 0; j < 8; j++)
                                {
                                    if (transactionhashesW[i].get()[j] == hash.get()[j])
                                    {
                                        trnsIndex = c; // find length of plaintext transaction data
                                        ciphertextTr = cph;
                                        trnsKey = ckey;
                                        correctPlaintext = plaintext;
                                        goto stop;
                                    }
                                }
                            }
                        }
                    }
                stop:
                    std::cout << "\ndecrypted transaction data:\t" << correctPlaintext;
                    break;
                }
                else if (userInput == "del-wallet")
                {
                    if (walletMap.empty())
                    {
                        std::cout << "\nno wallet found";
                        exit(EXIT_FAILURE);
                    }
                    std::shared_ptr<uint64_t> unverifiedWalletAddress(new uint64_t[8]);
                    std::shared_ptr<uint8_t> unverifiedWalletk2(new uint8_t[8]);
                    std::shared_ptr<uint8_t> unverifiedWalletk1(new uint8_t[8]);
                    std::map<std::shared_ptr<uint64_t>, std::vector<std::shared_ptr<uint8_t>>>
                        unverifiedWalletMap;
                    std::map<std::shared_ptr<uint64_t>, std::vector<std::shared_ptr<uint8_t>>>::
                        iterator ItUWMMap = unverifiedWalletMap.begin();
                    std::string str_wallet_ad;
                    std::string unverifiedStrWalletk1;
                    std::string unverifiedStrWalletk2;

                    std::cout << "verify user by inputting both wallet keys and walletAddress"
                              << "\ninput walletAddress:\t";
                    std::cin >> str_wallet_ad;
                    unverifiedWalletAddress = usrInWallet512(str_wallet_ad);
                    std::cout << "\n\ninput first key of wallet:\t";
                    std::cin >> unverifiedStrWalletk1;
                    unverifiedWalletk1 = aesKeyToSPtr<uint8_t>(unverifiedStrWalletk1);
                    std::cout << "\n\ninput second key of wallet:\t";
                    std::cin >> unverifiedStrWalletk2;
                    unverifiedWalletk2 = aesKeyToSPtr<uint8_t>(unverifiedStrWalletk2);
                    std::vector<std::shared_ptr<uint8_t>> unverifiedWalletVec;
                    unverifiedWalletVec.push_back(unverifiedWalletk1);
                    unverifiedWalletVec.push_back(unverifiedWalletk2);
                    unverifiedWalletMap.insert(ItUWMMap, std::pair<std::shared_ptr<uint64_t>,
                                                                   std::vector<std::shared_ptr<uint8_t>>>(unverifiedWalletAddress, unverifiedWalletVec));

                    // verify inputted wallet data
                    struct Wallet unv_wallet
                    {
                        unverifiedWalletAddress, unverifiedWalletVec,
                            unverifiedWalletMap
                    };
                    wallet_address.verifyInputWallet(walletAddresses, unverifiedWalletAddress);
                    unv_wallet.verifyOwnerData();

                    // delete wallet
                    std::string confirm;
                    if (storedCrypto != 0)
                    {
                        std::cout << "\n\nare you sure you want to delete wallet? Your balance is "
                                  << storedCrypto << ". You cannot recover your balance after"
                                  << "deletion\ntype d or delete for delete, any key for"
                                  << "terminating process:\t";
                    }
                    else
                    {
                        std::cout << "\n\nAre you sure you want to delete wallet?\nYour balance"
                                  << " is 0.\ntype \"d\" or \"delete\" for delete, "
                                  << "type anything for terminating process:\t";
                    }
                    std::cin >> confirm;
                    if (confirm == "d" || confirm == "delete")
                    {
                        std::cout << "\ndeleting wallet...\n";
                        for (int c = 0; c < 8; c++)
                            walletMap.clear();
                        transactions.clear();
                        trnsLengths.clear();
                        userAESmapkeys.clear();
                        AESkeysTr.clear();
                        std::string ciphertextW = "";
                        std::string ciphertextK1 = "";
                        std::string ciphertextK2 = "";
                        std::string usedEncAlg = "";

                        // delete wallet address from walletAddresses
                        for (int i = 0; i < walletAddresses.size(); i++)
                        {
                            for (int c = 0; c < 8; c++)
                            {
                                if (walletAddresses[i].get()[c] == walletAddress.get()[c])
                                {
                                    walletAddresses.erase(walletAddresses.begin() + i);
                                    goto stop_find;
                                }
                            }
                        }
                    stop_find:
                        walletAddress.reset();
                        std::cout << "wallet deleted";
                    }
                    else
                    {
                        std::cout << "\nprocess terminated";
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                else if (userInput == "exit" || userInput == "quit")
                {
                    std::cout << "\nprogram terminated";
                    exit(EXIT_FAILURE);
                }
                else if (userInput == "burn")
                {
                    if (!walletMap.empty())
                    {
                        std::string strWalletKey1;
                        std::shared_ptr<uint8_t> walletKey1(new uint8_t[32]);
                        std::string strWalletKey2;
                        std::shared_ptr<uint8_t> walletKey2(new uint8_t[32]);
                        std::vector<std::shared_ptr<uint8_t>> walletKeysVec;
                        std::map<std::shared_ptr<uint64_t>, std::vector<std::shared_ptr<uint8_t>>> unv_wallet_map;
                        std::map<std::shared_ptr<uint64_t>, std::vector<std::shared_ptr<uint8_t>>>::iterator unv_it_map = unv_wallet_map.begin();

                        std::cout << "\nverify wallet data to burn crypto\ninput wallet key 1:\t";
                        std::cin >> strWalletKey1;
                        walletKey1 = aesKeyToSPtr<uint8_t>(strWalletKey1);
                        std::cout << "input wallet key 2:\t";
                        std::cin >> strWalletKey2;
                        walletKey2 = aesKeyToSPtr<uint8_t>(strWalletKey2);

                        // append wallet keys to walletKeysVec for verification process
                        walletKeysVec.push_back(walletKey1);
                        walletKeysVec.push_back(walletKey2);

                        // verify wallet
                        for (const auto [wa, walletKeys] : walletMap)
                        {
                            unv_wallet_map.insert(unv_it_map, std::pair<std::shared_ptr<uint64_t>, std::vector<std::shared_ptr<uint8_t>>>(wa, walletKeysVec));
                            struct Wallet unv_wallet
                            {
                                wa, walletKeysVec, unv_wallet_map
                            };
                            unv_wallet.verifyOwnerData();
                            wallet_address.verifyInputWallet(walletAddresses, wa);
                            walletAddress = wa;
                        }

                        // create fake wallet address
                        auto [fakeWalletAd, fakeKeys] = wallet_address.GenerateNewWalletAddress();
                        bool walletAValid = wallet_address.verifyInputWallet(walletAddresses,
                                                                             fakeWalletAd);

                        // if fake wallet address exists, create new wallet address
                        while (walletAValid)
                        {
                            auto [newFakeWalletAd, newFakeKeys] = wallet_address.GenerateNewWalletAddress();
                            fakeWalletAd = newFakeWalletAd;
                            wallet_address.verifyInputWallet(walletAddresses, fakeWalletAd);
                        }

                        // burn
                        uint32_t amountBurn;
                        std::cout << "\ninput amount to burn:\t";
                        std::cin >> amountBurn;

                        // get accont balance
                        struct userData user_data
                        {
                            walletMap, transactions, transactionhashesW,
                                trnsLengths
                        };
                        storedCrypto = user_data.setBalance();

                        // unv_wallet_map is now verified
                        struct Wallet trWallet
                        {
                            walletAddress, walletKeysVec, unv_wallet_map
                        };

                        // create transaction
                        auto [newWA, newKeys] = trWallet.new_transaction(fakeWalletAd,
                                                                         walletAddress,
                                                                         amountBurn, mempool,
                                                                         "sell",
                                                                         transactionhashesW,
                                                                         transactions,
                                                                         storedCrypto,
                                                                         trnsLengths,
                                                                         all_trns_lengths);

                        // append transactions of single wallet to all transactions
                        // ciphertexts in blockchain
                        for (const auto [ciphertxt, trns_keys] : transactions)
                        {
                            // add transaction ciphertext and keys if its not already in
                            // transactions_enc
                            if (transactions_enc.find(ciphertxt) == transactions_enc.end())
                            {
                                transactions_enc.insert(it_trns_enc, std::pair<std::string,
                                                                               std::shared_ptr<uint8_t>>(ciphertxt,
                                                                                                         trns_keys));
                            }
                        }

                        // delete fake wallet address
                        fakeWalletAd.reset();
                    }
                    else
                    {
                        std::cout << "\nno wallet address found";
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                else if (userInput == "hash-sha512")
                {
                    std::string tmp_str;
                    std::cout << "\ninput text to hash using sha512:\t";
                    std::cin >> tmp_str;
                    std::string hash_sha512_str = to8_64_str(sha512(tmp_str));
                    std::cout << "\n\nhash:\t" << hash_sha512_str << std::endl;
                    break;
                }
                else if (userInput == "enc-aes128 -genkey" || userInput == "enc-aes192 -genkey" ||
                         userInput == "enc-aes256 -genkey" || userInput == "enc-aes128" ||
                         userInput == "enc-aes192" || userInput == "enc-aes256" ||
                         userInput == "dec-aes128" || userInput == "dec-aes192" ||
                         userInput == "dec-aes256")
                {
                    // find which algorithm algorithm
                    unsigned short algorithmSize;
                    bool withKey;
                    std::string key_size_str = "";
                    std::string plaintext;
                    std::string ciphertext;
                    std::string encOrDec = "";
                    std::stringstream ss;
                    for (int c = 0; c < 3; c++)
                    { // encryption or decryption
                        encOrDec += userInput[c];
                    }

                    // find algorithm size in bits
                    for (int c = 7; c <= 9; c++)
                    {
                        key_size_str += userInput[c];
                    }
                    ss << key_size_str;
                    ss >> algorithmSize;

                    if (encOrDec == "enc")
                    {
                        /* if userInput length bigger than 10, automatic key generation is
                         * requested
                         */
                        if (userInput.length() > 10)
                        {
                            withKey = true;
                        }
                        else
                        {
                            withKey = false;
                        }
                        std::shared_ptr<uint8_t> aesKeyEnc(new uint8_t[algorithmSize / 8]);

                        std::cout << "\nencrypting input using aes" << algorithmSize
                                  << ". input what to encrypt:\t";
                        std::cin >> plaintext;
                        if (!withKey)
                        {
                            std::string aesKeyEncStr;
                            std::cout << "\ninput 32 byte aes" << algorithmSize
                                      << " key as hex:\t";
                            std::cin >> aesKeyEncStr;
                            aesKeyEnc = aesKeyToSPtr<uint8_t>(aesKeyEncStr, algorithmSize / 8);
                            if (algorithmSize == 128)
                            {
                                ciphertext = aes128.encrypt(plaintext, aesKeyEnc);
                            }
                            else if (algorithmSize == 192)
                            {
                                ciphertext = aes192.encrypt(plaintext, aesKeyEnc);
                            }
                            else
                            { // 256
                                ciphertext = aes256.encrypt(plaintext, aesKeyEnc);
                            }
                        }
                        else
                        {
                            if (algorithmSize == 128)
                            {
                                aesKeyEnc = generateAES128Key();
                                ciphertext = aes128.encrypt(plaintext, aesKeyEnc);
                            }
                            else if (algorithmSize == 192)
                            {
                                aesKeyEnc = generateAES192Key();
                                ciphertext = aes192.encrypt(plaintext, aesKeyEnc);
                            }
                            else
                            { // 256
                                aesKeyEnc = generateAES256Key();
                                ciphertext = aes256.encrypt(plaintext, aesKeyEnc);
                            }
                        }

                        std::cout << "ciphertext:\t" << ciphertext << "\n\naes"
                                  << algorithmSize << " key:\t";
                        std::string aesKeyEncStr = aesKeyToStr<uint8_t>(aesKeyEnc,
                                                                        algorithmSize / 8);
                        std::cout << aesKeyEncStr;
                    }
                    else
                    {
                        std::string aesKeyDecStr;
                        std::shared_ptr<uint8_t> aesKeyDec(new uint8_t[algorithmSize / 8]);
                        std::cout << "\nnote: if plaintext of the encrypted text is "
                                  << "not a multiple of 16, there will be padding with"
                                  << " zeros at the end of the decrypted ciphertext"
                                  << " because aes algorithms encrypt plaintext as"
                                  << " blocks of 16 bytes, if you know the length of"
                                  << " plaintext, delete the padding of \"0\"\'s"
                                  << "\n\ninput ciphertext:\t";
                        std::cin >> ciphertext;
                        std::cout << "\n\ninput aes" << algorithmSize << " key:\t";
                        std::cin >> aesKeyDecStr;
                        aesKeyDec = aesKeyToSPtr<uint8_t>(aesKeyDecStr, algorithmSize / 8);

                        if (algorithmSize == 128)
                        {
                            plaintext = aes128.decrypt(ciphertext, aesKeyDec);
                        }
                        else if (algorithmSize == 192)
                        {
                            plaintext = aes192.decrypt(ciphertext, aesKeyDec);
                        }
                        else
                        { // 256
                            plaintext = aes256.decrypt(ciphertext, aesKeyDec);
                        }
                        std::cout << "\n\nplaintext:\t" << plaintext;
                    }
                    break;
                }
                else if (userInput == "get myahr")
                {
                    uint32_t accuracy;
                    std::cout << "input accuracy of hashrate (how many seconds should"
                              << " the calculation last?)\ninput in decimal format"
                              << "(no floating points):\t";
                    std::cin >> accuracy;
                    std::cout << "\ncalculating average hashrate...";
                    uint64_t hashrate = Blockchain::calcHashRateSha512(accuracy);
                    std::cout << "\nyour hashrate:\t" << std::dec << hashrate;
                    break;
                }
                else if (userInput == "get blockchain")
                {
                    std::cout << "printing all blocks in the blockchain...";
                    if (blockchain.empty())
                    {
                        std::cout << "\nno blocks in blockchain, type "
                                  << "\"start mine\" to start mining";
                    }
                    else
                    {
                        std::cout << "blockchain:\t";
                        for (auto it = blockchain.begin(); it !=
                                                           blockchain.end();
                             ++it)
                        {
                            std::cout << *it << std::endl
                                      << std::endl;
                        }
                    }
                    break;
                }
                else if (userInput.starts_with("get block-hash"))
                {
                    const auto [block_index, NaN] = ui::get_block_index_ui(userInput);
                    if (NaN)
                    {
                        break;
                    }
                    else
                    {
                        if (blockhashes.empty())
                        {
                            std::cout << "\nno blockhashes found";
                        }
                        else
                        {
                            if (blockhashes.size() < block_index)
                            {
                                std::cout << "\nindex bigger than blockchain size"
                                          << " (index starts from zero)";
                            }
                            else
                            {
                                std::cout << "\nblock hash:\t"
                                          << to8_64_str(blockhashes[block_index])
                                          << std::endl;
                            }
                        }
                        break;
                    }
                }
                else if (userInput == "get block-nonce")
                {
                    const auto [index, NaN] = ui::get_block_index_ui(userInput);
                    if (NaN)
                    {
                        break;
                    }
                    else
                    {
                        if (blockchain.size() == 0 && index >= blockchain.size())
                        {
                            std::cout << "blockchain size smaller than " << index
                                      << ".\n";
                        }
                        else
                        {
                            std::cout << "\nfinding block nonce...\n";
                            std::set<std::string>::iterator itBlock = blockchain.begin();
                            std::string block = *std::next(blockchain.begin(), index);
                            std::string str_nonce = block.substr(block.find("nonce: "),
                                                                 block.find("\ndifficulty"));
                            std::cout << "nonce:\t" << str_nonce;
                        }
                        break;
                    }
                }
                else if (userInput == "get block-timestamp")
                {
                    const auto [index, NaN] = ui::get_block_index_ui(userInput);
                    if (NaN)
                    {
                        break;
                    }
                    else
                    {
                        if (!blockchain.empty())
                        {
                            std::cout << "\nfinding block timestamp...\n";
                            std::set<std::string>::iterator itBlock = blockchain.begin();
                            std::string block = *std::next(blockchain.begin(), index);
                            std::string str_time = block.substr(block.find("timestamp: "),
                                                                block.find("\nblockchain size"));
                            std::cout << "timestamp:\t" << str_time;
                        }
                        else
                        {
                            std::cout << "\nblockchain empty\n";
                        }
                        break;
                    }
                }
                else if (userInput == "get block-merkle-r")
                {
                    const auto [index, NaN] = ui::get_block_index_ui(userInput);
                    if (NaN)
                    {
                        break;
                    }
                    else
                    {
                        std::string str_merkle_root;
                        if (!blockchain.empty())
                        {
                            std::cout << "\nfinding block merkle root...\n";
                            std::set<std::string>::iterator itBlock = blockchain.begin();
                            std::string block = *std::next(blockchain.begin(), index);
                            str_merkle_root = block.substr(block.find("merkle_root: "),
                                                           block.find("\napproximate time until next block"));
                            std::cout << "merkle_root:\t" << str_merkle_root;
                        }
                        else
                        {
                            std::cout << "\nblockchain empty\n";
                        }
                        break;
                    }
                }
                else if (userInput == "get block-difficulty")
                {
                    const auto [index, NaN] = ui::get_block_index_ui(userInput);
                    if (NaN)
                    {
                        break;
                    }
                    else
                    {
                        std::string str_difficulty;
                        if (!blockchain.empty())
                        {
                            std::cout << "\nfinding block difficulty...\n";
                            std::set<std::string>::iterator itBlock = blockchain.begin();
                            std::string block = *std::next(blockchain.begin(), index);
                            str_difficulty = block.substr(block.find("difficulty: "),
                                                          block.find("\nmerkle_root"));
                            std::cout << "difficulty:\t" << str_difficulty;
                        }
                        else
                        {
                            std::cout << "\nblockchain empty\n";
                        }
                        break;
                    }
                }
                else if (userInput == "get block-ahr")
                {
                    const auto [index, NaN] = ui::get_block_index_ui(userInput);
                    if (NaN)
                    {
                        break;
                    }
                    else
                    {
                        std::string hashrate_str;
                        if (!blockchain.empty())
                        {
                            std::cout << "\nfinding block hashrate...\n";
                            std::set<std::string>::iterator itBlock = blockchain.begin();
                            std::string block = *std::next(blockchain.begin(), index);
                            hashrate_str = block.substr(block.find("Average hashrate of miners: "),
                                                        block.find("\nblockchain version"));
                            std::cout << "hashrate:\t" << hashrate_str
                                      << std::endl;
                        }
                        else
                        {
                            std::cout << "\nblockchain empty\n";
                        }
                        break;
                    }
                }
                else if (userInput == "get nblocktime")
                {
                    const auto [index, NaN] = ui::get_block_index_ui(userInput);
                    if (NaN)
                    {
                        break;
                    }
                    else
                    {
                        std::string nblocktime_str;
                        if (!blockchain.empty())
                        {
                            std::cout << "\nfinding next block generation time...\n";
                            std::set<std::string>::iterator itBlock = blockchain.begin();
                            std::string block = *std::next(blockchain.begin(), index);
                            nblocktime_str = block.substr(block.find("approximate time until next block: "),
                                                          block.find("\nAverage hashrate of miners"));
                            std::cout << "next block gen time:\t" << nblocktime_str
                                      << std::endl;
                        }
                        else
                        {
                            std::cout << "\nblockchain empty\n";
                        }
                        break;
                    }
                }
                else if (userInput == "get blockchain-size")
                {
                    std::cout << "blockchain size:\t" << blockchain.size()
                              << std::endl;
                }
                else if (userInput == "get version")
                {
                    std::cout << "\nversion of blockchain core:\t" << blockchain_version
                              << std::endl;
                }
                else if (userInput == "get mempool")
                {
                    std::cout << "mempool size:\t" << mempool.size() << std::endl;
                    if (mempool.size() == 0)
                    {
                        std::cout << "\nmempool empty";
                    }
                    else
                    {
                        std::cout << "\ndumping mempool...\n";

// delay print for user to see mempool size
#ifdef OS_WINDOWS
                        Sleep(3000); // windows sleep function
#else
                        usleep(3000000); // unix sleep function
#endif
                    }
                    for (int i = 0; i < mempool.size(); i++)
                        std::cout << std::endl
                                  << to8_64_str(mempool[i]);
                    break;
                }
                else if (userInput == "enc-algs")
                {
                    std::cout << "\nAES128\tAES192\tAES256" << std::endl;
                }
                else if (userInput == "start mine")
                {
                    std::cout << "\nstarting mining\n";
                    if (!block_mined)
                    {
                        std::tuple<std::shared_ptr<uint64_t>, std::string, uint32_t, uint64_t,
                                   double, std::shared_ptr<uint64_t>, double, double>
                            unverified_block_data = block.data(unsafe_mempool);
                        uint32_t blockchainSize;
                        uint64_t nonce;
                        std::shared_ptr<uint64_t> prevBlockHash(new uint64_t[8]);
                        std::string timestamp;
                        double difficulty, nextBlockGenTime, avHashrate;
                        std::tie(prevBlockHash, timestamp, blockchainSize, nonce, difficulty,
                                 merkle_root, nextBlockGenTime, avHashrate) = unverified_block_data;
                        auto [isblockmined, clean_mempool] = ProofofWork.mineBlock(transactions_enc,
                                                                                   nonce, difficulty,
                                                                                   mempool,
                                                                                   merkle_root,
                                                                                   all_trns_lengths);
                        std::cout << "\nmempool cleaned";
                        block_mined = isblockmined;

                        if (block_mined)
                        {
                            std::cout << "\nblock mined successfully";
                            std::cout << "\nrepresenting correct block in blockhain...\n\n";
                            std::string current_block = block.data_str(prevBlockHash,
                                                                       timestamp,
                                                                       blockchainSize,
                                                                       nonce, difficulty,
                                                                       nextBlockGenTime,
                                                                       avHashrate,
                                                                       clean_mempool,
                                                                       blockchain_version);
                            std::cout << current_block;
                            blockchain.insert(current_block);
                            std::cout << "\n\nblock added to blockchain";
                            /* wrong mempool cannot have less than correct mempool since wrong
                             * mempool has new false transaction, if there is a modified
                             * transaction hash, it won't work, therefore needs further updates.
                             * More functionality will be added in further versions
                             */
                            std::cout << "\n\nclean mempool: \n";
                            for (int i = 0; i < clean_mempool.size(); i++)
                            {
                                for (int c = 0; c < 8; c++)
                                    std::cout << std::hex << clean_mempool[i].get()[c];
                                std::cout << std::endl;
                            }
                            mempool = clean_mempool;
                            if (!walletMap.empty())
                            {
                                storedCrypto += 100;
                                std::cout << "added 100 to your balance. You know own "
                                          << storedCrypto << ".";
                            }
                            else
                            {
                                std::cout << "wallet map empty, cannot add";
                            }
                        }
                    }
                }
                else if (userInput == "dump-wallet512")
                {
                    if (walletMap.empty())
                    {
                        std::cout << "\nno wallet address found";
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        std::cout << "\nwallet address:\t";
                        for (const auto [wa, walletKeys] : walletMap)
                        {
                            for (int c = 0; c < 8; c++)
                                std::cout << std::hex << wa.get()[c];
                        }
                    }
                    std::cout << std::endl
                              << std::endl
                              << "if you want to also"
                              << " see wallet keys, type \"get p-w keys\"";
                    break;
                }
                else if (userInput == "show c")
                {
                    ui::show_c_command();
                    break;
                }
                else if (userInput == "show w")
                {
                    ui::show_w_command();
                    break;
                }
                else if (userInput == "")
                {
                    break;
                }
                else
                {
                    std::cout << "command not found\n";
                    break;
                }
            }
        }
    }
} /* namespace ui */

#endif /* UI_H_ */
