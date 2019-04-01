//
//  UartModuleViewController.swift
//  Basic Chat
//
//  Created by Trevor Beaton on 12/4/16.
//  Copyright © 2016 Vanguard Logic LLC. All rights reserved.
//





import UIKit
import CoreBluetooth

class UartModuleViewController: UIViewController, CBPeripheralManagerDelegate, UITextViewDelegate, UITextFieldDelegate {
    
    //UI
    @IBOutlet weak var baseTextView: UITextView!
    @IBOutlet weak var sendButton: UIButton!
    @IBOutlet weak var inputTextField: UITextField!
    @IBOutlet weak var scrollView: UIScrollView!
    @IBOutlet weak var device1Name: UILabel!
    @IBOutlet weak var readValue: UILabel!
    @IBOutlet weak var readButton: UIButton!
    
    @IBOutlet weak var send2Button: UIButton!
    @IBOutlet weak var read2Button: UIButton!
    @IBOutlet weak var read2Value: UILabel!
    @IBOutlet weak var input2TextField: UITextField!
    @IBOutlet weak var device2Name: UILabel!
    @IBOutlet weak var base2TextView: UITextView!
    //Data
    var peripheralManager: CBPeripheralManager?
    var peripherals: [CBPeripheral?] = []
    //private var consoleAsciiText:NSAttributedString? = NSAttributedString(string: "")
    private var device1Text:NSAttributedString? = NSAttributedString(string: "")
    private var device2Text:NSAttributedString? = NSAttributedString(string: "")
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        self.navigationItem.backBarButtonItem = UIBarButtonItem(title:"Back", style:.plain, target:nil, action:nil)
        self.baseTextView.delegate = self
        self.inputTextField.delegate = self
        //Base1 text view setup
        self.baseTextView.layer.borderWidth = 3.0
        self.baseTextView.layer.borderColor = UIColor.blue.cgColor
        self.baseTextView.layer.cornerRadius = 3.0
        self.baseTextView.text = ""
        //Base2 text view setup
        self.base2TextView.layer.borderWidth = 3.0
        self.base2TextView.layer.borderColor = UIColor.blue.cgColor
        self.base2TextView.layer.cornerRadius = 3.0
        self.base2TextView.text = ""
        //Input Text Field setup
        self.inputTextField.layer.borderWidth = 2.0
        self.inputTextField.layer.borderColor = UIColor.blue.cgColor
        self.inputTextField.layer.cornerRadius = 3.0
        
        //Create and start the peripheral manager
        peripheralManager = CBPeripheralManager(delegate: self, queue: nil)
        //-Notification for updating the text view with incoming text
        
        self.device1Name.text = peripherals[0]!.name!
        self.device2Name.text = peripherals[1]!.name!
        
        //updateIncomingData()
    }
    
    override func viewDidAppear(_ animated: Bool) {
        self.baseTextView.text = ""
        self.base2TextView.text = ""
        
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        // peripheralManager?.stopAdvertising()
        // self.peripheralManager = nil
        super.viewDidDisappear(animated)
        NotificationCenter.default.removeObserver(self)
        
    }
    
    func updateIncomingData (d_Num: Int) {
        if rxCharacteristic[d_Num] != nil {
            let data = rxCharacteristic[d_Num]!.value
            var byte = [UInt8](repeating:0, count:data!.count)
            data!.copyBytes(to: &byte, count: data!.count)
            myString = "\(byte)"
            if (d_Num==0){
                self.readValue.text=myString
            }
            else if (d_Num==1){
                self.read2Value.text=myString
            }
            NotificationCenter.default.post(name:NSNotification.Name(rawValue: "Notify"), object: nil)
        }
        NotificationCenter.default.addObserver(forName: NSNotification.Name(rawValue: "Notify"), object: nil , queue: nil){
            notification in
            let appendString = "\n"
            let myFont = UIFont(name: "Helvetica Neue", size: 15.0)
            let myAttributes2 = [NSFontAttributeName: myFont!, NSForegroundColorAttributeName: UIColor.red]
            
            let attribString = NSAttributedString(string: "[Incoming]: " + myString + appendString, attributes: myAttributes2)
            
            if (d_Num==0){
                let newAsciiText = NSMutableAttributedString(attributedString: self.device1Text!)
                self.baseTextView.attributedText = NSAttributedString(string: characteristicASCIIValue as String , attributes: myAttributes2)
                newAsciiText.append(attribString)
                self.device1Text = newAsciiText
                self.baseTextView.attributedText = self.device1Text
                //self.baseTextView.attributedText = attribString
            }
            else if (d_Num==1){
                let newAsciiText = NSMutableAttributedString(attributedString: self.device2Text!)
                self.baseTextView.attributedText = NSAttributedString(string: characteristicASCIIValue as String , attributes: myAttributes2)
                newAsciiText.append(attribString)
                self.device2Text = newAsciiText
                self.base2TextView.attributedText = self.device2Text
                //self.base2TextView.attributedText = attribString
            }
        }
    }
    
    /*
    @objc func Notify(_ notification: Notification){
        updateIncomingData(d_Num: deviceIndex)
    }
    */
    
    @IBAction func clickSendAction(_ sender: AnyObject) {
        outgoingData(deviceNum: 0)
        deviceIndex = 0;
    }
    @IBAction func clickSend2Action(_ sender: Any) {
        outgoingData(deviceNum: 1)
        deviceIndex = 1;
    }
    
    @IBAction func clickReadAction(_ sender: Any) {
        updateIncomingData(d_Num: 0)
        deviceIndex = 0;
    }
    @IBAction func clickRead2Action(_ sender: Any) {
        print("Read2 Button")
        updateIncomingData(d_Num: 1)
        deviceIndex = 1;
    }
    
    func outgoingData (deviceNum: Int) {
        let inputText = inputTextField.text // Get the user inputted value
        
        var bytes: [UInt8] = Array(inputText!.utf8) // Convert String to byte array
        bytes[0] = bytes[0] - 48 //Account for the fact that string was converted from      ASCII to decimal. This will need to be update to a for loop.
        let myData = Data(bytes: bytes) //Convert byte array to data
        
        print(bytes) //Print the value you are sending
        print(myData) // Print the amount of bytes you are sending
        
        //Send the data to the characteristic
        if let blePeripheral = peripherals[deviceNum]{
            if let txCharacteristic = txCharacteristic[deviceNum] {
                blePeripheral.writeValue(myData, for: txCharacteristic, type: CBCharacteristicWriteType.withResponse)
            }
        }
        
        //writeValue(data: inputText!) NLU
        
        // Format and write the value we sent to the UI
        let appendString = "\n"
        let myFont = UIFont(name: "Helvetica Neue", size: 15.0)
        let myAttributes1 = [NSFontAttributeName: myFont!, NSForegroundColorAttributeName: UIColor.blue]
        
        let attribString = NSAttributedString(string: "[Outgoing]: " + inputText! + appendString, attributes: myAttributes1)
        
        if (deviceNum==0){
            let newAsciiText = NSMutableAttributedString(attributedString: self.device1Text!)
            newAsciiText.append(attribString)
            self.device1Text = newAsciiText
            baseTextView.attributedText = device1Text
        }
        else if (deviceNum==1){
            let newAsciiText = NSMutableAttributedString(attributedString: self.device2Text!)
            newAsciiText.append(attribString)
            
            self.device2Text = newAsciiText
            baseTextView.attributedText = device2Text
        }
        
    }
    
    // Write functions -- Am I actually calling this function??
    func writeValue(data: String, device_Num: Int){
        let valueString = (data as NSString).data(using: String.Encoding.utf8.rawValue)
        //change the "data" to valueString
        if let blePeripheral = blePeripherals[device_Num]{
            if let txCharacteristic = txCharacteristic[device_Num] {
                blePeripheral.writeValue(valueString!, for: txCharacteristic, type: CBCharacteristicWriteType.withResponse)
            }
        }
    }
    
    func writeCharacteristic(val: Int8, device_Num: Int){
        var val = val
        let ns = NSData(bytes: &val, length: MemoryLayout<Int8>.size)
        blePeripherals[device_Num]!.writeValue(ns as Data, for: txCharacteristic[device_Num]!, type: CBCharacteristicWriteType.withResponse)
    }
    
    
    
    //MARK: UITextViewDelegate methods
    func textViewShouldBeginEditing(_ textView: UITextView) -> Bool {
        if (textView === baseTextView) || (textView === base2TextView) {
            //tapping on consoleview dismisses keyboard
            inputTextField.resignFirstResponder()
            return false
        }
        return true
    }
    
    func textFieldDidBeginEditing(_ textField: UITextField) {
        // This pushes the keyboard up when how hit the send field. Disabled for the first device.
        scrollView.setContentOffset(CGPoint(x:0, y:250), animated: true)
    }
    
    func textFieldDidEndEditing(_ textField: UITextField) {
        scrollView.setContentOffset(CGPoint(x:0, y:0), animated: true)
    }
    
    func peripheralManagerDidUpdateState(_ peripheral: CBPeripheralManager) {
        if peripheral.state == .poweredOn {
            return
        }
        print("Peripheral manager is running")
    }
    
    //Check when someone subscribe to our characteristic, start sending the data
    func peripheralManager(_ peripheral: CBPeripheralManager, central: CBCentral, didSubscribeTo characteristic: CBCharacteristic) {
        print("Device subscribe to characteristic")
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        self.view.endEditing(true)
    }
    
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        textField.resignFirstResponder()
        outgoingData(deviceNum: 0)
        return(true)
    }
    
    func peripheralManagerDidStartAdvertising(_ peripheral: CBPeripheralManager, error: Error?) {
        if let error = error {
            print("\(error)")
            return
        }
    }
}

