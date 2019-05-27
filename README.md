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
cmake .. -G"Visual Studio 15 2017 Win64"
msbuild couchbase-lite-cpp.sln /p:Configuration=RelWithDebInfo
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