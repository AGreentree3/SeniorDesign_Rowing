//
//  PeripheralsListController.swift
//  OarTracker
//
//  Created by Abby Greentree on 4/1/19.
//  Copyright © 2019 Abby Greentree. All rights reserved.
//

import UIKit
import CoreBluetooth

//Global Variables
var PeripheralsPage = PeripheralsListController()

class PeripheralsListController: UIViewController, UITableViewDelegate, UITableViewDataSource {

    //Outlets
    @IBOutlet weak var doneButton: UIBarButtonItem!
    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var backButton: UIBarButtonItem!
    
    //Variables
    var peripherals: [CBPeripheral?] = []
    
    //Variables
    let cellID: String = "PeripheralsViewCell"
    
    override func viewDidLoad() {
        super.viewDidLoad()
        tableView.delegate = PeripheralsPage
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
        return peripherals.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        guard let cell = tableView.dequeueReusableCell(withIdentifier: cellID, for: indexPath) as? PeripheralsViewCell else{
            fatalError("The dequeued cell is not an instance of Peripherals ViewCell.")
        }
        
        let dev = peripherals[indexPath.row]
        cell.deviceName.text = dev!.name!
        //cell.RSSI.text = String(dev.RSSI)

        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.cellForRow(at: indexPath)?.accessoryType = UITableViewCell.AccessoryType.checkmark
        connectedDevices.append(peripherals[indexPath.row])
    }
    
    
}
