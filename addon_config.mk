meta:
    ADDON_NAME = ofxRabbitControl
    ADDON_DESCRIPTION = addon for RabbitControl
    ADDON_AUTHOR = Ingo Randolf
    ADDON_TAGS = "Utilities"
    ADDON_URL = https://github.com/rabbitControl/

common:
	# dependencies with other addons, a list of them separated by spaces
	# or use += in several lines
#	ADDON_DEPENDENCIES =

	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=
	ADDON_INCLUDES += libs/
