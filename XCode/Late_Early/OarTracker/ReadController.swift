//
//  ReadController.swift
//  OarTracker
//
//  Created by Abby Greentree on 4/1/19.
//  Copyright © 2019 Abby Greentree. All rights reserved.
//

import UIKit
import CoreBluetooth

//Global Variables
var ReadPage = ReadController() //Instance of the view controller
var strokeState: Int = 0 //This is a needed global variable so that all oars can check their state against the stroke seat (1st oar) state

class ReadController: UIViewController, UITableViewDelegate, UITableViewDataSource{

    //Outlets
    @IBOutlet weak var readController: UISegmentedControl!
    //Outlets
    @IBAction func readTypeChange(_ sender: Any) {
        //This function is called when the user clicks from 'Single Read' to 'Continuous Read' or vice versa
        changeReadState()
    }
    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var backButton: UIBarButtonItem!
    
    //Variables
    let cellID: String = "ReadViewCell"
    //var peripheralManager: CBPeripheralManager?
    //var peripherals: [CBPeripheral?] = []
    var testCheck: Bool = false //Variable for testing purposes
    var timer1: Timer? //Timer that will be used for continuous read
    
    override func viewDidLoad() {
        super.viewDidLoad()
        tableView.delegate = ReadPage
        tableView.dataSource = ReadPage
        //peripheralManager = CBPeripheralManager(delegate: self, queue: nil)
    }
    
    func changeReadState(){
        let totNum = connectedDevices.count - 1
        var buttonString: String = ""
        
        //Perform a switch statement on the read type that the user has selected
        switch readController.selectedSegmentIndex {
        case 0: //Case 0 is Single Read
            buttonString = "Read" //All buttons should say read
        case 1: //Case 1 is Continuous Read
            buttonString = "Start" //Buttons should say start instead of read
            cycleRead() // When the user starts continuous read call this function to actually start the read process
        default:
            buttonString = "Error"
        }
        //This for loop just changes the button titles and the read state. Read state is a variable used in the ReadViewCell.
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
        //The for loop goes through each device connected
        for n in 0...totNum {
            let index = IndexPath(row: n, section: 0)
            if let cell = tableView.cellForRow(at: index) as? ReadViewCell {
                if(cell.goContinuous){ //If the cell has been set to continuous reading
                    allOff = false //Set the flag to false so the timer keeps firing
                    cell.readValue(deviceIndex: cell.deviceNum) //Call the read value function defined in ReadViewCell
                }
            }
        }
        if(allOff){ //If no device is set to continous reading then this allOff variable will remain true throughout the for loop and we should kill the timer to stop continuous reading 
            timer1?.invalidate()
        }
    }
    
    func cycleRead(){
        //This function starts continuous reading.
        DispatchQueue.main.asyncAfter(deadline: .now() + 3.0) { //This line waits three seconds after the user switches to continuous read. The only reason we wait three seconds is because the code needs time to finish updating the read state on each of the cells, if we don't wait three seconds than we received a segfault.
            self.timer1 = Timer.scheduledTimer(timeInterval: 0.001, target: self, selector: #selector(self.fireTimer), userInfo: nil, repeats: true) // Declare a continuously running timer for 10 milliseconds(0.001), when the time fires it will call the fireTimer function that is defined above. This timer will keep firing every 10 milliseconds unless it is killed.
            print("Dispatched")
        }
    }
}


