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

    @IBOutlet weak var readButton: UIButton!
    @IBOutlet weak var deviceName: UILabel!
    @IBOutlet weak var charValue: UILabel!
    @IBOutlet weak var stateLabel: UILabel!
    
    // Variables
    var deviceNum: Int = 0
    var readState: Int = 0
    var goContinuous: Bool = false
    var isStroke: Bool = false
    /*
    var previousState = 0
    var strokeInterval = 0
    var strokePM = 0
    let timer = Timer.scheduledTimer(timeInterval: 1.0, target: self, selector: #selector(fire), userInfo: nil, repeats: true)
    */
    
    @IBAction func clickRead(_ sender: Any) {
        if readState == 0 {
            readValue(deviceIndex: self.deviceNum)
        }
        else if readState == 1{
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
        if rxCharacteristic != nil {
            let data = rxCharacteristic[deviceNum]!.value
            //print(data)
            var byte = [UInt8](repeating:0, count:data!.count)
            data!.copyBytes(to: &byte, count: data!.count)
            myString = "\(byte)"
            //print(myString)
            //charValue.text = myString
        }
        switch myString {
            case "[0]":
                //previousState = 0
                self.charValue.backgroundColor = UIColor(red:0.93, green:0.78, blue:0.26, alpha: 0)
                charValue.text = "      "
                if (isStroke && strokeState != 0){
                    strokeState = 0
                }
                else if (!isStroke && strokeState != 0){
                    self.stateLabel.text = "Late!"
                }
                else{
                    self.stateLabel.text = ""
                }
            case "[1]":
                /*
                if (previousState == 0){
                    strokePM = 60/strokeInterval
                    print(strokePM, "SPM")
                    //timer.invalidate()
                    strokeInterval = 0
                    //let timer = Timer.scheduledTimer(timeInterval: 1.0, target: self, selector: #selector(fire), userInfo: nil, repeats: true)
                }
                */
                self.charValue.backgroundColor = UIColor(red:0.93, green:0.78, blue:0.26, alpha:1.0)
                charValue.text = "Catch:"
                if (isStroke && strokeState != 1){
                    strokeState = 1
                }
                else if (!isStroke && strokeState != 1){
                    self.stateLabel.text = "Early!"
                }
                else if(!isStroke){
                    self.stateLabel.text = "On Time"
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
    
    /*
    @objc func fire()
    {
        strokeInterval = strokeInterval+1
    }
    */

}
