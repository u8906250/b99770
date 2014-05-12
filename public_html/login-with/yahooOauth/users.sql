
CREATE TABLE IF NOT EXISTS `users` (
  `userid` int(11) NOT NULL auto_increment,
  `Guid` varchar(50) NOT NULL,
  `username` varchar(50) default NULL,
  `Full Name` varchar(50) default NULL,
  `Email` varchar(50) NOT NULL,
  PRIMARY KEY  (`userid`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;