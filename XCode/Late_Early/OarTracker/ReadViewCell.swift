//
//  ReadViewCell.swift
//  OarTracker
//
//  Created by Abby Greentree on 4/3/19.
//  Copyright © 2019 Abby Greentree. All rights reserved.
//

import Foundation
import UIKit
import CoreBluetooth

class ReadViewCell: UITableViewCell, UITextViewDelegate{
//These are the cells for the read view page
    
    //Outlets
    @IBOutlet weak var readButton: UIButton!
    @IBOutlet weak var deviceName: UILabel!
    @IBOutlet weak var charValue: UILabel!
    @IBOutlet weak var stateLabel: UILabel!
    
    // Variables
    var deviceNum: Int = 0
    var readState: Int = 0
    var goContinuous: Bool = false
    var isStroke: Bool = false
    
    @IBAction func clickRead(_ sender: Any) {
        if readState == 0 { //If we are in single read mode than call the read value function for just this device
            readValue(deviceIndex: self.deviceNum)
        }
        else if readState == 1{ //If we are in continuous read mode than clicking this button starts or stops continuous readin gfor this specific oar
            if !goContinuous{
                readButton.setTitle("Stop", for: .normal)
                goContinuous = true
                //cycleRead()
            } else {
                readButton.setTitle("Start", for: .normal)
                goContinuous = false
            }
        }
    }
    
    func readValue(deviceIndex: Int){
        //Read Value Function that read the characteristic value for the device
        if rxCharacteristic != nil {
            //This is the same code from the old BLE app
            let data = rxCharacteristic[deviceNum]!.value
            var byte = [UInt8](repeating:0, count:data!.count)
            data!.copyBytes(to: &byte, count: data!.count)
            myString = "\(byte)"
            print(myString)
            charValue.text = myString //The value read is outputted to a label
        }
        
        //The following switch case is what set the late or early labels.
        switch charValue.text {
            case "[0]": // 0 = NOT the catch
                if (isStroke && strokeState != 0){
                    strokeState = 0 //If this is the stroke seat oar and the value has changed that update the strokeState variable to reflect the change
                }
                else if (!isStroke && strokeState != 0){
                    self.stateLabel.text = "Late!"  //If this oar is not the stroke seat and the stroke seat has alread hit the catch then this oar is late
                }
                else{
                    self.stateLabel.text = ""
                }
            case "[1]": // 1 = the catch
                if (isStroke && strokeState != 1){
                    strokeState = 1 //If this is the stroke seat oar and the value has changed that update the strokeState variable to reflect the change
                }
                else if (!isStroke && strokeState != 1){
                    self.stateLabel.text = "Early" //If this oar is not the stroke seat and the stroke seat has not already hit the catch then this oar is early
                }
                else{
                    self.stateLabel.text = ""
                }
            default:
                self.stateLabel.text = ""
        }
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
        self.charValue.text = ""
        self.stateLabel.text = ""
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

}
