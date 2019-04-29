//
//  UUIDKey.swift
//  OarTracker
//
//  Created by Abby Greentree on 4/3/19.
//  Copyright © 2019 Abby Greentree. All rights reserved.
//

import Foundation
import CoreBluetooth


//UUID for TI Button Service (not advertised in Project Zero)
let kButtonService_UUID = "FFF0"
let kButton1_Characteristic_uuid_Rx = "FFF4"
//let kButton2_Characteristic_uuid_Rx = "F0001122-0451-4000-B000-000000000000"

//UUID for TI LED service (not advertised in Project Zero)
let kLEDService_UUID = "F0001110-0451-4000-B000-000000000000"
let kLED_Characteristic_uuid_Tx = "F0001111-0451-4000-B000-000000000000"
let kLED_Characteristic_uuid_Rx = "F0001112-0451-4000-B000-000000000000"

//UUID That is actually being advertised in simple peripheral
let kAdvert_UUID = "FFF0"

let MaxCharacters = 20

let Advert_UUID = CBUUID(string: kAdvert_UUID)
let Advert2_UUID = CBUUID(string: "6e400001-b5a3-f393-e0a9-e50e24dcca9e")

let RX_UUID = CBUUID(string: kButtonService_UUID)
let Button1_Characteristic_uuid_Rx = CBUUID(string: kButton1_Characteristic_uuid_Rx)// (Property = Read from Button 1)
//let Button2_Characteristic_uuid_Rx = CBUUID(string: kButton2_Characteristic_uuid_Rx)// (Property = Read from Button 2)

let TX_UUID = CBUUID(string: kLEDService_UUID)
let LED_Characteristic_uuid_Tx = CBUUID(string: kLED_Characteristic_uuid_Tx)// (Property = Read from Button 1)
let LED_Characteristic_uuid_Rx = CBUUID(string: kLED_Characteristic_uuid_Rx)// (Property = Read from Button 2)

//REFERENCES:

//Using simple peripheral program:
//let kBLEService_UUID = "FFF0" // Peripheral has to be advertising this specific UUID
//let kBLE_Characteristic_uuid_Tx = "0000FFF1-0000-1000-8000-00805F9B34FB"
//let kBLE_Characteristic_uuid_Rx = "0000FFF2-0000-1000-8000-00805F9B34FB"

//UUID for TI LED service (not advertised in Project Zero)
//let kBLEService_UUID = "F0001110-0451-4000-B000-000000000000"
//let kBLE_Characteristic_uuid_Tx = "F0001111-0451-4000-B000-000000000000"
//let kBLE_Characteristic_uuid_Rx = "F0001112-0451-4000-B000-000000000000"

//UUID for TI Button service (not advertised in Project Zero)
//let kBLEService_UUID = "F0001120-0451-4000-B000-000000000000"
//let kBLE_Characteristic_uuid_Tx = "F0001121-0451-4000-B000-000000000000"
//let kBLE_Characteristic_uuid_Rx = "F0001122-0451-4000-B000-000000000000"

//UUIDs for the Adafruit device:
//let kBLEService_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
//let kBLE_Characteristic_uuid_Tx = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
//let kBLE_Characteristic_uuid_Rx = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"


