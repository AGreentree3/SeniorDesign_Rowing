//
//  ConnectOarsController.swift
//  OarTracker
//
//  Created by Abby Greentree on 4/1/19.
//  Copyright © 2019 Abby Greentree. All rights reserved.
//

import Foundation
import UIKit
import CoreBluetooth

var txCharacteristic : [CBCharacteristic?] = []
var rxCharacteristic : [CBCharacteristic?] = []
//var blePeripherals : [CBPeripheral?] = []
var connectedDevices : [CBPeripheral?] = []
var characteristicASCIIValue = NSString()
var myString: String = "Null"
var numDevices: Int = 2
var deviceIndex: Int = 0


var ConnectPage = ConnectOarsController()

class ConnectOarsController : UIViewController, CBPeripheralDelegate, CBCentralManagerDelegate {

    @IBOutlet weak var oarNum: UILabel!
    @IBOutlet weak var connectButton: UIButton!
    @IBOutlet weak var finishedButton: UIButton!
    
    @IBAction func clickConnect(_ sender: Any) {
        performSegue(withIdentifier: "ConnecttoPeripherals", sender: self)
    }
    @IBAction func clickFinished(_ sender: Any) {
        performSegue(withIdentifier: "ConnecttoComm", sender: self)
        let totNum = connectedDevices.count - 1
        for n in 0...totNum {
            connectToDevice(deviceNum: n)
        }
    }
    
    //Data
    var centralManager : CBCentralManager!
    var RSSIs = [NSNumber]()
    var data = NSMutableData()
    var writeData: String = ""
    var peripherals: [CBPeripheral?] = []
    var characteristicValue = [CBUUID: NSData]()
    var timer = Timer()
    var characteristics = [String : CBCharacteristic]()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        centralManager = CBCentralManager(delegate: self, queue: nil)
        self.oarNum.text = "0 Oars"
        
        //Visual Setup
        connectButton.layer.cornerRadius = 5
        connectButton.layer.borderWidth = 1
        finishedButton.layer.cornerRadius = 5
        finishedButton.layer.borderWidth = 1
        
        finishedButton.isEnabled = false
        finishedButton.setTitleColor(UIColor.gray, for: .disabled)
        
    }
    
    override func viewDidAppear(_ animated: Bool) {
        //disconnectAllConnection()
        super.viewDidAppear(animated)
        //refreshScanView()
        print("View Cleared")
        self.oarNum.text = String(connectedDevices.count) + " Oars"
        if (connectedDevices.count != 0){
            finishedButton.isEnabled = true
        }
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        print("Stop Scanning")
        centralManager?.stopScan()
    }
    
    /*Okay, now that we have our CBCentalManager up and running, it's time to start searching for devices. You can do this by calling the "scanForPeripherals" method.*/
    
    func startScan() {
        peripherals = []
        print("Now Scanning...")
        self.timer.invalidate()
        centralManager?.scanForPeripherals(withServices: [Advert_UUID] , options: [CBCentralManagerScanOptionAllowDuplicatesKey:false])
        Timer.scheduledTimer(timeInterval: 7, target: self, selector: #selector(self.cancelScan), userInfo: nil, repeats: false)
    }
    
    @objc func cancelScan() {
        self.centralManager?.stopScan()
        print("Scan Stopped")
        print("Number of Peripherals Found: \(peripherals.count)")
        //blePeripherals = peripherals
    }
    
    func disconnectFromDevice (deviceNum: Int) {
        centralManager.cancelPeripheralConnection(peripherals[deviceNum]!)
    }
    
    
    func restoreCentralManager() {
        //Restores Central Manager delegate if something went wrong
        centralManager?.delegate = self
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral,advertisementData: [String : Any], rssi RSSI: NSNumber) {
        self.peripherals.append(peripheral)
        //blePeripherals.append(peripheral)
        self.RSSIs.append(RSSI)
        //peripheral.delegate = self
        //self.baseTableView.reloadData()
        print("Found new pheripheral devices with services")
        print("Peripheral name: \(String(describing: peripheral.name))")
        print("**********************************")
        print ("Advertisement Data : \(advertisementData)")
    }
    
    func connectToDevice (deviceNum: Int) {
        centralManager?.connect(connectedDevices[deviceNum]!, options: nil)
        deviceIndex=deviceNum+1
    }

    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("*****************************")
        print("Connection complete")
        print("Peripheral info: \(String(describing: peripheral))")
        
        //Stop Scan- We don't need to scan once we've connected to a peripheral. We got what we came for.
        centralManager?.stopScan()
        print("Scan Stopped")
        
        //Erase data that we might have
        data.length = 0
        
        //Discovery callback
        peripheral.delegate = self
        //Only look for services that matches transmit uuid
        peripheral.discoverServices([Advert_UUID, RX_UUID, TX_UUID])
    }
    
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        if error != nil {
            print("Failed to connect to peripheral")
            return
        }
    }
    
    func disconnectAllConnection() {
        for blePeripheral in peripherals {
            if let blePeripheral = blePeripheral {
                centralManager?.cancelPeripheralConnection(blePeripheral)
            }
        }
        deviceIndex=0
    }
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == CBManagerState.poweredOn {
            // We will just handle it the easy way here: if Bluetooth is on, proceed...start scan!
            print("Bluetooth Enabled")
            startScan()
            
        } else {
            //If Bluetooth is off, display a UI alert message saying "Bluetooth is not enable" and "Make sure that your bluetooth is turned on"
            print("Bluetooth Disabled- Make sure your Bluetooth is turned on")
            
            let alertVC = UIAlertController(title: "Bluetooth is not enabled", message: "Make sure that your bluetooth is turned on", preferredStyle: UIAlertController.Style.alert)
            let action = UIAlertAction(title: "ok", style: UIAlertAction.Style.default, handler: { (action: UIAlertAction) -> Void in
                self.dismiss(animated: true, completion: nil)
            })
            alertVC.addAction(action)
            self.present(alertVC, animated: true, completion: nil)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        print("Disconnected")
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == "ConnecttoPeripherals"{
            cancelScan()
            PeripheralsPage.peripherals = peripherals
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        print("*******************************************************")
        
        if ((error) != nil) {
            print("Error discovering services: \(error!.localizedDescription)")
            return
        }
        
        guard let services = peripheral.services else {
            return
        }
        //We need to discover the all characteristic
        for service in services {
            
            peripheral.discoverCharacteristics(nil, for: service)
            // bleService = service -> why is this commented out?
        }
        print("Discovered Services: \(services)")
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        
        print("*******************************************************")
        
        if ((error) != nil) {
            print("Error discovering services: \(error!.localizedDescription)")
            return
        }
        
        guard let characteristics = service.characteristics else {
            return
        }
        
        print("Found \(characteristics.count) characteristics!")
        
        for characteristic in characteristics {
            //looks for the right characteristic
            //print(characteristic.uuid)
            if characteristic.uuid.isEqual(Button1_Characteristic_uuid_Rx)  {
                rxCharacteristic.append(characteristic)
                
                //Once found, subscribe to the this particular characteristic...
                peripheral.setNotifyValue(true, for: characteristic)
                // We can return after calling CBPeripheral.setNotifyValue because CBPeripheralDelegate's
                // didUpdateNotificationStateForCharacteristic method will be called automatically
                peripheral.readValue(for: characteristic)
                print("Rx Characteristic: \(characteristic.uuid)")
            }
            if characteristic.uuid.isEqual(LED_Characteristic_uuid_Tx){
                txCharacteristic.append(characteristic)
                print("Tx Characteristic: \(characteristic.uuid)")
            }
            peripheral.discoverDescriptors(for: characteristic)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverDescriptorsFor characteristic: CBCharacteristic, error: Error?) {
        print("*******************************************************")
        
        if error != nil {
            print("\(error.debugDescription)")
            return
        }
        if ((characteristic.descriptors) != nil) {
            
            for x in characteristic.descriptors!{
                let descript = x as CBDescriptor!
                print("function name: DidDiscoverDescriptorForChar \(String(describing: descript?.description))")
                //print("Rx Value \(String(describing: rxCharacteristic?.value))")
                //print("Tx Value \(String(describing: txCharacteristic?.value))")
            }
        }
    }
    
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
        print("*******************************************************")
        
        if (error != nil) {
            print("Error changing notification state:\(String(describing: error?.localizedDescription))")
            
        } else {
            print("Characteristic's value subscribed")
        }
        
        if (characteristic.isNotifying) {
            print ("Subscribed. Notification has begun for: \(characteristic.uuid)")
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
        guard error == nil else {
            print("Error discovering services: error")
            return
        }
        print("Message sent")
    }
    
    func peripheral(_ peripheral: CBPeripheral, didWriteValueFor descriptor: CBDescriptor, error: Error?) {
        guard error == nil else {
            print("Error discovering services: error")
            return
        }
        print("Succeeded!")
    }
    
}

