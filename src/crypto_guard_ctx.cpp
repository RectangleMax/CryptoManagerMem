#include "../include/crypto_guard_ctx.h"


namespace CryptoGuard {

    struct CryptoGuardCtx::Impl {
// Интерфейсные методы 
        void EncryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
            DoCryptFileManager(inStream, outStream, password, 1);       
        }

        void DecryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
            DoCryptFileManager(inStream, outStream, password, 0);
        }

        std::string CalculateChecksum(std::iostream&);

// Служебные методы и параметры
        void DoCryptFile(std::iostream& inStream, std::iostream& outStream, std::string_view password, int encrypt_flag);

        void DoCryptFileManager(std::iostream&, std::iostream&, std::string_view, int enc);        

        struct AesCipherParams {
            static const size_t KEY_SIZE = 32;             // AES-256 key size
            static const size_t IV_SIZE = 16;              // AES block size (IV length)
            const EVP_CIPHER *cipher = EVP_aes_256_cbc();  // Cipher algorithm
//            int encrypt;                              // 1 for encryption, 0 for decryption
            std::array<unsigned char, KEY_SIZE> key;  // Encryption key
            std::array<unsigned char, IV_SIZE> iv;    // Initialization vector
        };
        AesCipherParams CreateChiperParamsFromPassword(std::string_view);

        size_t BUF_SIZE = 4096;
        size_t getStreamSize(std::fstream&);
        size_t getStreamSize(std::stringstream&);
    };
    using Impl_ = CryptoGuardCtx::Impl;


// Определение методов класса-интерфейса
    void CryptoGuardCtx::EncryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
        pImpl.get()->EncryptFile(inStream, outStream, password);
    }

    void CryptoGuardCtx::DecryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
        pImpl.get()->DecryptFile(inStream, outStream, password);
    }

    std::string CryptoGuardCtx::CalculateChecksum(std::iostream &stream) { 
        if (auto *pStream = dynamic_cast<std::fstream*>(&stream); pStream) {
            return pImpl->CalculateChecksum(*pStream);
        } else if (auto *pStream = dynamic_cast<std::stringstream*>(&stream); pStream) {
            return pImpl->CalculateChecksum(*pStream);
        } else {
            throw std::runtime_error("Недопустимый тип потока ввода (iostream&) для расчёта контрольной суммы");
        }
    }


// Определение одного из "интерфейсных" методов Impl-класса (определение двух других - вместе с объявлением)
    std::string Impl_::CalculateChecksum(std::iostream& stream) { 
        if (stream.fail())
            throw std::runtime_error("Проблема с чтением потока ввода (файл не найден или недоступен)."); 

        struct MDCtxDeleter {
            void operator()(EVP_MD_CTX* ptr) { EVP_MD_CTX_free(ptr); }
        };
        std::unique_ptr<EVP_MD_CTX, MDCtxDeleter> MDContext(EVP_MD_CTX_new());
1
        if (EVP_DigestInit_ex(MDContext.get(), EVP_sha256(), nullptr) != 1) {
            throw std::runtime_error("Ошибка подсчёта контрольной суммы: Error initializing SHA-256 digest");
        }

        int i = 0;
        std::vector<unsigned char> buf(BUF_SIZE);
        while (true) {
            stream.read(reinterpret_cast<char*>(buf.data()), BUF_SIZE);
            size_t bytes_reading = stream.gcount();
            if (!bytes_reading)
                break;
            if (!stream.good()  &&  !stream.eof())
                throw std::runtime_error("Проблема с чтением потока ввода."); 
            
            if (EVP_DigestUpdate(MDContext.get(), buf.data(), bytes_reading) != 1) 
                throw std::runtime_error("Ошибка подсчёта контрольной суммы: Error updating digest");
        }

        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int lengthOfHash = 0;
        if (EVP_DigestFinal_ex(MDContext.get(), hash, &lengthOfHash) != 1) {
            throw std::runtime_error("Ошибка подсчёта контрольной суммы: Error finalizing digest");
        }

        // Преобразование бинарного хеша в hex-строку
        std::stringstream ss;
        for (unsigned int i = 0; i < lengthOfHash; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }
        return ss.str();
    }



// Определение "внутренних" методов Impl-класса
    CryptoGuardCtx::CryptoGuardCtx() : pImpl(std::make_unique<Impl>()) { 
        OpenSSL_add_all_algorithms(); 
    }
    CryptoGuardCtx::~CryptoGuardCtx() { EVP_cleanup(); }

    Impl_::AesCipherParams Impl_::CreateChiperParamsFromPassword(std::string_view password) {
        Impl_::AesCipherParams params;
        constexpr std::array<unsigned char, 8> salt = {'1', '2', '3', '4', '5', '6', '7', '8'};
    
        int result = EVP_BytesToKey(params.cipher, EVP_sha256(), salt.data(),
                                    reinterpret_cast<const unsigned char *>(password.data()), password.size(), 1,
                                    params.key.data(), params.iv.data());
    
        if (result == 0) {
            throw std::runtime_error{"Ошибка шифрования: Failed to create a key from password"};
        }
    
        return params;
    }


    void Impl_::DoCryptFileManager(std::iostream &inStream, std::iostream &outStream, std::string_view password, int encrypt_flag) {
        std::vector<std::iostream*> in_out_streams(2);
        for (int i = 0; i < 2; ++i) {
            in_out_streams[i] = dynamic_cast<std::stringstream*>( i == 0 ? &inStream : &outStream) ;
            if (in_out_streams[i] == nullptr) {
                in_out_streams[i]  = dynamic_cast<std::fstream*>( i == 0 ? &inStream : &outStream);
                if (in_out_streams[i] == nullptr) {
                    throw std::runtime_error("Недопустимый тип потока " + std::string( i == 0 ? "ввода" : "вывода") + " (iostream&).");
                }
            } 
        }
        Impl_::DoCryptFile(*in_out_streams[0], *in_out_streams[1], password, encrypt_flag);
    }


    void Impl_::DoCryptFile(std::iostream& inStream, std::iostream& outStream, std::string_view password, int encrypt_flag) {
        if (inStream.fail())
            throw std::runtime_error("Проблема с чтением потока ввода (файл не найден или недоступен)."); 

        struct CipherCtxDeleter {
            void operator()(EVP_CIPHER_CTX* ptr) { EVP_CIPHER_CTX_free(ptr); }
        };
        std::unique_ptr<EVP_CIPHER_CTX, CipherCtxDeleter> CipherContext(EVP_CIPHER_CTX_new());

        auto params = CreateChiperParamsFromPassword(password);
        EVP_CipherInit_ex(CipherContext.get(), params.cipher, nullptr, params.key.data(), params.iv.data(), encrypt_flag);       

        // Подготовка к чтению/записи    
        std::vector<unsigned char>  inBuf(BUF_SIZE);
        std::vector<unsigned char> outBuf(BUF_SIZE + EVP_MAX_BLOCK_LENGTH);

        while (true) {
            // Чтение входного буфера            
            inStream.read(reinterpret_cast<char*>(inBuf.data()), BUF_SIZE);
            int bytes_read = inStream.gcount();
            if (!bytes_read)
                break; // Входной буфер закончен 
            if (!inStream.good()  &&  !inStream.eof()) { 
                throw std::runtime_error("Проблема с чтением потока ввода."); 
            }

            // Шифрование / дешифрование
            int bytes_encrypted = 0;
            if (!EVP_CipherUpdate(CipherContext.get(), outBuf.data(), &bytes_encrypted, inBuf.data(), bytes_read)) {
                throw std::runtime_error("Ошибка при шифровании данных.");
            }

            // Запись шифрованнх данных в выходной буфер
            if (!outStream.good()) { 
                throw std::runtime_error("Проблема с записью в поток вывода."); 
            }
            outStream.write(reinterpret_cast<const char*>(outBuf.data()), bytes_encrypted);
        }

        int final_length = 0;
        if (!EVP_CipherFinal_ex(CipherContext.get(), outBuf.data(), &final_length)) {
            throw std::runtime_error("Ошибка при шифровании данных: Finalization failed");
        }
        outStream.write(reinterpret_cast<const char*>(outBuf.data()), final_length);
    }


    size_t getStreamSize(std::fstream& fs) {
        fs.seekg(0, std::ios::end);
        size_t size = fs.tellg();
        fs.seekg(0, std::ios::beg);
        return size;
    }
    size_t getStreamSize(std::stringstream& ss) {
        return ss.str().size();
    }
}  
