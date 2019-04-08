//
//  ReadController.swift
//  OarTracker
//
//  Created by Abby Greentree on 4/1/19.
//  Copyright © 2019 Abby Greentree. All rights reserved.
//

import UIKit
import CoreBluetooth

var ReadPage = ReadController()
var strokeState: Int = 0

class ReadController: UIViewController, UITableViewDelegate, UITableViewDataSource{

    @IBOutlet weak var readController: UISegmentedControl!
    //Outlets
    @IBAction func readTypeChange(_ sender: Any) {
        changeReadState()
    }
    
    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var backButton: UIBarButtonItem!
    
    //Variables
    let cellID: String = "ReadViewCell"
    //var peripheralManager: CBPeripheralManager?
    //var peripherals: [CBPeripheral?] = []
    var testCheck: Bool = false
    var timer1: Timer?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        tableView.delegate = ReadPage
        tableView.dataSource = ReadPage
        //peripheralManager = CBPeripheralManager(delegate: self, queue: nil)
    }
    
    func changeReadState(){
        let totNum = connectedDevices.count - 1
        var buttonString: String = ""
        switch readController.selectedSegmentIndex {
        case 0:
            buttonString = "Read"
        case 1:
            buttonString = "Start"
            cycleRead()
        default:
            buttonString = "Error"
        }
        for n in 0...totNum {
            let index = IndexPath(row: n, section: 0)
            let cell = tableView.cellForRow(at: index) as! ReadViewCell
            print(readController.selectedSegmentIndex)
            cell.readState = readController.selectedSegmentIndex
            cell.readButton.setTitle(buttonString, for: .normal)
            print("For Loop")
        }
    }

    override func viewDidAppear(_ animated: Bool) {
        
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        // peripheralManager?.stopAdvertising()
        // self.peripheralManager = nil
        super.viewDidDisappear(animated)
        NotificationCenter.default.removeObserver(ReadPage)
        
    }
    
    @IBAction func clickBack(_ sender: Any) {
        performSegue(withIdentifier: "readBack", sender: ReadPage)
    }
    
    func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return connectedDevices.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        guard let cell = tableView.dequeueReusableCell(withIdentifier: cellID, for: indexPath) as? ReadViewCell else{
            fatalError("The dequeued cell is not an instance of ReadViewCell.")
        }
        
        let dev = connectedDevices[indexPath.row]
        
        cell.deviceName.text = dev!.name!
        cell.deviceNum = indexPath.row
        //cell.RSSI.text = String(dev.RSSI)
        if indexPath.row == 0{
            cell.isStroke = true;
        }
        
        return cell
    }
    
    @objc func fireTimer() {
        let totNum = connectedDevices.count - 1
        var allOff: Bool = true
        for n in 0...totNum {
            let index = IndexPath(row: n, section: 0)
            if let cell = tableView.cellForRow(at: index) as? ReadViewCell {
                if(cell.goContinuous){
                    allOff = false
                    cell.readValue(deviceIndex: cell.deviceNum)
                }
            }
        }
        if(allOff){
            timer1?.invalidate()
        }
    }
    
    func cycleRead(){
        DispatchQueue.main.asyncAfter(deadline: .now() + 3.0) {
            // your code here
            self.timer1 = Timer.scheduledTimer(timeInterval: 0.001, target: self, selector: #selector(self.fireTimer), userInfo: nil, repeats: true)
            print("Dispatched")
        }
    }
}


