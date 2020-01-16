# Couhbase-lite-cpp library
This library depends on couchbase-lite-core. 

Note: This is tested on Mac OS, Windows, and Unix (Ubuntu, CentOS).

# Build
```git submodule update --init --recursive```
- MacOS, Linux
```
mkdir build && cd build
cmake ..
make -j4
```
- Windows
```
mkdir build
cd build
cmake .. -G"NMake Makefiles" -DCMAKE_BUILD_TYPE="DEBUG"
nmake
```

# Build with SSL enabled (optional)
This library offers support for secure Couchbase replication through SSL connections if OpenSSL is installed in the host machine. At the moment, this is supported/tested only on Mac OS and Windows. To achieve this, compile this wrapper against OpenSSL which will provide SSL and encryption features, as demonstraded below:

## Mac OS step-by-step
- Install OpenSSL@1.1 with the Homebrew package manager:
``` brew install openssl@1.1 ```
- Determine where OpenSSL@1.1 libraries were installed
- On current Mac OS systems this defaults to
``` /usr/local/opt/openssl@1.1/lib ```
- Compile Couchbase-lite-cpp linking against the 'libssl.1.1.dylib' and 'libcrypto.1.1.dylib' dynamic library files using the "SSL_LIB" and "CRYPTO_LIB" CMake variables. If OpenSSL was installed in the location shown above, the command for this will be:
(Note the need to escape the quotes in the directory variable)
```
mkdir build && cd build
cmake -DSSL_LIB="\"/usr/local/opt/openssl@1.1/lib/libssl.1.1.dylib\"" -DCRYPTO_LIB="\"/usr/local/opt/openssl@1.1/lib/libcrypto.1.1.dylib\"" ..
make -j4
```

## Windows step-by-step
- Install OpenSSL@1.1, either by downloading the binaries or by installing "Git for Windows", which ships with OpenSSL.
- The required files are "libssl-1_1-x64.dll" and "libcrypto-1_1-x64.dll". If installed with "Git for Windows", these are located within C:\Program Files\Git\mingw64\bin".
- Add "C:\Program Files\Git\mingw64\bin" to the system path.
- Compile Couchbase-lite-cpp linking against "libssl-1_1-x64.dll" and "libcrypto-1_1-x64.dll" dynamic library files using the "SSL_LIB" and "CRYPTO_LIB" CMake variables. In this case, the command will be:
```
mkdir build
cd build
cmake .. -G"NMake Makefiles" -DCMAKE_BUILD_TYPE="DEBUG" -DSSL_LIB="\"libssl-1_1-x64.dll\"" -DCRYPTO_LIB="\"libcrypto-1_1-x64.dll\""
nmake
```


# Examples
Run the example demos from ./build/examples
```
./fleece-playground
./sgcouchbaselite-playground
```

DB location will be inside build/db/${dbname}/db.sqlite3.
The db can be viewed using sqlitebrowser.

# Couchbase backend technologies
- Install Couchbase server from `https://www.couchbase.com/downloads`. 
This library was tested with Couchbase version `5.5.1`
- Install Sync Gateway 2.1.1 from the above link
- Follow sync gateway instructions setup from `https://docs.couchbase.com/sync-gateway/2.1/getting-started.html`
- NOTE: Use the following configuration file and change information as needed.
```JSON
{
    "log": ["*"],
    "databases": {
        "staging": {
            "server": "http://localhost:8091",
            "bucket": "staging",
            "username": "sync_gateway",
            "password": "sync_gateway",
            "enable_shared_bucket_access": true,
            "import_docs": "continuous",
            "num_index_replicas": 0,
            "users": {
              "GUEST": {
                "disabled": false, "admin_channels": ["*"]
              },
              "username": {
                "disabled": false, "admin_channels": ["*"], "password":"password"
              }
            }
        }
    }
}
```

# TODO:
1. Tests.
2. Logging.
