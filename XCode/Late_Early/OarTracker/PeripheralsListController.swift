//
//  PeripheralsListController.swift
//  OarTracker
//
//  Created by Abby Greentree on 4/1/19.
//  Copyright © 2019 Abby Greentree. All rights reserved.
//
//  This page will show the user the devices that are available to connect to
// This page is linked with 'PeripheralsViewCell.swift' this code will call instances of Peripheral View Cell to populate the table for the user to see

import UIKit
import CoreBluetooth

//Global Variables
//This is an instance of the List Controller
var PeripheralsPage = PeripheralsListController()

class PeripheralsListController: UIViewController, UITableViewDelegate, UITableViewDataSource {

    //Outlets
    @IBOutlet weak var doneButton: UIBarButtonItem!
    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var backButton: UIBarButtonItem!
    
    //Variables
    var peripherals: [CBPeripheral?] = []
    // The cellID identifies the view cell that we want to the table to be populated with. The cells must be defined as 'PeripheralsViewCell in the code file and in the storyboard
    let cellID: String = "PeripheralsViewCell"
    
    override func viewDidLoad() {
        super.viewDidLoad()
        tableView.delegate = PeripheralsPage //set the delegate to the instance of the view controller that was declared globally
        tableView.dataSource = PeripheralsPage
    }
    
    @IBAction func clickDone(_ sender: Any) {
        performSegue(withIdentifier: "peripheralsBack", sender: self)
    }
    
    @IBAction func clickBack(_ sender: Any) {
        performSegue(withIdentifier: "peripheralsBack", sender: self)
    }
    
    func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        //You need the number of rows that you have peripheral devices
        return peripherals.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        //This is calling each instance of the view cell.
        guard let cell = tableView.dequeueReusableCell(withIdentifier: cellID, for: indexPath) as? PeripheralsViewCell else{
            fatalError("The dequeued cell is not an instance of Peripherals ViewCell.")
        }
        
        // For each peripheral device in the table, set the values needed in Peripheral View Cell
        let dev = peripherals[indexPath.row]
        cell.deviceName.text = dev!.name!
        //cell.RSSI.text = String(dev.RSSI)

        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.cellForRow(at: indexPath)?.accessoryType = UITableViewCell.AccessoryType.checkmark
        //When a user tables on the peripheral add it to the connectedDevices array
        connectedDevices.append(peripherals[indexPath.row])
    }
    
    
}
