//
//  PeripheralsViewCell.swift
//  OarTracker
//
//  Created by Abby Greentree on 4/3/19.
//  Copyright © 2019 Abby Greentree. All rights reserved.
//

import UIKit

class PeripheralsViewCell: UITableViewCell {
//These are the cells for the peripherals list view page
    @IBOutlet weak var deviceName: UILabel!
    @IBOutlet weak var RSSI: UILabel!
    
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

}
