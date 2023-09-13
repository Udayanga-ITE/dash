// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SPENTINDEX_H
#define BITCOIN_SPENTINDEX_H

#include <addressindex.h>
#include <amount.h>
#include <script/script.h>
#include <serialize.h>
#include <uint256.h>

#include <tuple>

struct CSpentIndexKey {
public:
    uint256 txid;
    unsigned int outputIndex;

public:
    CSpentIndexKey(uint256 t, unsigned int i) {
        txid = t;
        outputIndex = i;
    }

    CSpentIndexKey() {
        SetNull();
    }

    void SetNull() {
        txid.SetNull();
        outputIndex = 0;
    }

public:
    SERIALIZE_METHODS(CSpentIndexKey, obj)
    {
        READWRITE(obj.txid, obj.outputIndex);
    }
};

struct CSpentIndexValue {
public:
    uint256 txid;
    unsigned int inputIndex;
    int blockHeight;
    CAmount satoshis;
    int addressType;
    uint160 addressHash;

public:
    CSpentIndexValue(uint256 t, unsigned int i, int h, CAmount s, int type, uint160 a) {
        txid = t;
        inputIndex = i;
        blockHeight = h;
        satoshis = s;
        addressType = type;
        addressHash = a;
    }

    CSpentIndexValue() {
        SetNull();
    }

    void SetNull() {
        txid.SetNull();
        inputIndex = 0;
        blockHeight = 0;
        satoshis = 0;
        addressType = AddressType::UNKNOWN;
        addressHash.SetNull();
    }

    bool IsNull() const {
        return txid.IsNull();
    }

public:
    SERIALIZE_METHODS(CSpentIndexValue, obj)
    {
        READWRITE(obj.txid, obj.inputIndex, obj.blockHeight, obj.satoshis, obj.addressType, obj.addressHash);
    }
};

struct CSpentIndexKeyCompare
{
    bool operator()(const CSpentIndexKey& a, const CSpentIndexKey& b) const {
        auto to_tuple = [](const CSpentIndexKey& obj) {
            return std::tie(obj.txid, obj.outputIndex);
        };
        return to_tuple(a) < to_tuple(b);
    }
};

struct CSpentIndexTxInfo
{
    std::map<CSpentIndexKey, CSpentIndexValue, CSpentIndexKeyCompare> mSpentInfo;
};

struct CTimestampIndexIteratorKey {
public:
    unsigned int timestamp;

public:
    CTimestampIndexIteratorKey(unsigned int time) {
        timestamp = time;
    }

    CTimestampIndexIteratorKey() {
        SetNull();
    }

    void SetNull() {
        timestamp = 0;
    }

public:
    size_t GetSerializeSize(int nType, int nVersion) const {
        return 4;
    }

    template<typename Stream>
    void Serialize(Stream& s) const {
        ser_writedata32be(s, timestamp);
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        timestamp = ser_readdata32be(s);
    }
};

struct CTimestampIndexKey {
public:
    unsigned int timestamp;
    uint256 blockHash;

public:
    CTimestampIndexKey(unsigned int time, uint256 hash) {
        timestamp = time;
        blockHash = hash;
    }

    CTimestampIndexKey() {
        SetNull();
    }

    void SetNull() {
        timestamp = 0;
        blockHash.SetNull();
    }

public:
    size_t GetSerializeSize(int nType, int nVersion) const {
        return 36;
    }

    template<typename Stream>
    void Serialize(Stream& s) const {
        ser_writedata32be(s, timestamp);
        blockHash.Serialize(s);
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        timestamp = ser_readdata32be(s);
        blockHash.Unserialize(s);
    }
};

struct CAddressUnspentKey {
public:
    unsigned int type;
    uint160 hashBytes;
    uint256 txhash;
    size_t index;

public:
    CAddressUnspentKey(unsigned int addressType, uint160 addressHash, uint256 txid, size_t indexValue) {
        type = addressType;
        hashBytes = addressHash;
        txhash = txid;
        index = indexValue;
    }

    CAddressUnspentKey() {
        SetNull();
    }

    void SetNull() {
        type = AddressType::UNKNOWN;
        hashBytes.SetNull();
        txhash.SetNull();
        index = 0;
    }

public:
    size_t GetSerializeSize(int nType, int nVersion) const {
        return 57;
    }

    template<typename Stream>
    void Serialize(Stream& s) const {
        ser_writedata8(s, type);
        hashBytes.Serialize(s);
        txhash.Serialize(s);
        ser_writedata32(s, index);
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        type = ser_readdata8(s);
        hashBytes.Unserialize(s);
        txhash.Unserialize(s);
        index = ser_readdata32(s);
    }
};

struct CAddressUnspentValue {
public:
    CAmount satoshis;
    CScript script;
    int blockHeight;

public:
    CAddressUnspentValue(CAmount sats, CScript scriptPubKey, int height) {
        satoshis = sats;
        script = scriptPubKey;
        blockHeight = height;
    }

    CAddressUnspentValue() {
        SetNull();
    }

    void SetNull() {
        satoshis = -1;
        script.clear();
        blockHeight = 0;
    }

    bool IsNull() const {
        return (satoshis == -1);
    }

public:
    SERIALIZE_METHODS(CAddressUnspentValue, obj)
    {
        READWRITE(obj.satoshis, obj.script, obj.blockHeight);
    }
};

struct CAddressIndexKey {
public:
    unsigned int type;
    uint160 hashBytes;
    int blockHeight;
    unsigned int txindex;
    uint256 txhash;
    size_t index;
    bool is_spent;

public:
    CAddressIndexKey(unsigned int addressType, uint160 addressHash, int height, int blockindex,
                     uint256 txid, size_t indexValue, bool isSpending) {
        type = addressType;
        hashBytes = addressHash;
        blockHeight = height;
        txindex = blockindex;
        txhash = txid;
        index = indexValue;
        is_spent = isSpending;
    }

    CAddressIndexKey() {
        SetNull();
    }

    void SetNull() {
        type = AddressType::UNKNOWN;
        hashBytes.SetNull();
        blockHeight = 0;
        txindex = 0;
        txhash.SetNull();
        index = 0;
        is_spent = false;
    }

public:
    size_t GetSerializeSize(int nType, int nVersion) const {
        return 66;
    }

    template<typename Stream>
    void Serialize(Stream& s) const {
        ser_writedata8(s, type);
        hashBytes.Serialize(s);
        // Heights are stored big-endian for key sorting in LevelDB
        ser_writedata32be(s, blockHeight);
        ser_writedata32be(s, txindex);
        txhash.Serialize(s);
        ser_writedata32(s, index);
        char f = is_spent;
        ser_writedata8(s, f);
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        type = ser_readdata8(s);
        hashBytes.Unserialize(s);
        blockHeight = ser_readdata32be(s);
        txindex = ser_readdata32be(s);
        txhash.Unserialize(s);
        index = ser_readdata32(s);
        char f = ser_readdata8(s);
        is_spent = f;
    }
};

struct CAddressIndexIteratorKey {
public:
    unsigned int type;
    uint160 hashBytes;

public:
    CAddressIndexIteratorKey(unsigned int addressType, uint160 addressHash) {
        type = addressType;
        hashBytes = addressHash;
    }

    CAddressIndexIteratorKey() {
        SetNull();
    }

    void SetNull() {
        type = AddressType::UNKNOWN;
        hashBytes.SetNull();
    }

public:
    size_t GetSerializeSize(int nType, int nVersion) const {
        return 21;
    }

    template<typename Stream>
    void Serialize(Stream& s) const {
        ser_writedata8(s, type);
        hashBytes.Serialize(s);
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        type = ser_readdata8(s);
        hashBytes.Unserialize(s);
    }
};

struct CAddressIndexIteratorHeightKey {
public:
    unsigned int type;
    uint160 hashBytes;
    int blockHeight;

public:
    CAddressIndexIteratorHeightKey(unsigned int addressType, uint160 addressHash, int height) {
        type = addressType;
        hashBytes = addressHash;
        blockHeight = height;
    }

    CAddressIndexIteratorHeightKey() {
        SetNull();
    }

    void SetNull() {
        type = AddressType::UNKNOWN;
        hashBytes.SetNull();
        blockHeight = 0;
    }

public:
    size_t GetSerializeSize(int nType, int nVersion) const {
        return 25;
    }

    template<typename Stream>
    void Serialize(Stream& s) const {
        ser_writedata8(s, type);
        hashBytes.Serialize(s);
        ser_writedata32be(s, blockHeight);
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        type = ser_readdata8(s);
        hashBytes.Unserialize(s);
        blockHeight = ser_readdata32be(s);
    }
};

#endif // BITCOIN_SPENTINDEX_H
