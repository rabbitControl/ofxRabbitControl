meta:
    ADDON_NAME = ofxRabbitControl
    ADDON_DESCRIPTION = addon for RabbitControl
    ADDON_AUTHOR = Ingo Randolf
    ADDON_TAGS = "Utilities"
    ADDON_URL = https://github.com/rabbitControl/ofxRabbitControl

# using boot asio, interprocess v 1.72.0

common:
	# dependencies with other addons, a list of them separated by spaces
	# or use += in several lines
#	ADDON_DEPENDENCIES =

	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=
	ADDON_INCLUDES += libs/

	# prevent boost to be used header only
	ADDON_DEFINES += BOOST_ASIO_SEPARATE_COMPILATION


osx:
	# ADDON_INCLUDES_EXCLUDE = libs/libwebsockets/include/win32port
	# ADDON_INCLUDES_EXCLUDE += libs/libwebsockets/include/win32port/win32helpers
	# OpenSSL support for OSX 10.11+
	ADDON_INCLUDES 	+= libs/openssl/include
	ADDON_LIBS 		+= libs/openssl/lib/osx/crypto.a
	ADDON_LIBS 		+= libs/openssl/lib/osx/ssl.a
