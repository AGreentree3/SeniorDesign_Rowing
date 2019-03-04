//
//  UUIDKey.swift
//  Basic Chat
//
//  Created by Trevor Beaton on 12/3/16.
//  Copyright © 2016 Vanguard Logic LLC. All rights reserved.
//

import CoreBluetooth
//Uart Service uuid

//Using simple peripheral program:
//let kBLEService_UUID = "FFF0" // Peripheral has to be advertising this specific UUID
//let kBLE_Characteristic_uuid_Tx = "0000FFF1-0000-1000-8000-00805F9B34FB"
//let kBLE_Characteristic_uuid_Rx = "0000FFF2-0000-1000-8000-00805F9B34FB"

//UUID for TI not advertised:
let kBLEService_UUID = "F0001110-0451-4000-B000-000000000000"
let kBLE_Characteristic_uuid_Tx = "F0001111-0451-4000-B000-000000000000"
let kBLE_Characteristic_uuid_Rx = "F0001112-0451-4000-B000-000000000000"

//UUIDs for the Adafruit device:
//let kBLEService_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
//let kBLE_Characteristic_uuid_Tx = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
//let kBLE_Characteristic_uuid_Rx = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

let MaxCharacters = 20

let BLEService_UUID = CBUUID(string: kBLEService_UUID)
let BLE_Characteristic_uuid_Tx = CBUUID(string: kBLE_Characteristic_uuid_Tx)//(Property = Write without response)
let BLE_Characteristic_uuid_Rx = CBUUID(string: kBLE_Characteristic_uuid_Rx)// (Property = Read/Notify)
