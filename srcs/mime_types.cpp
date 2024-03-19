#include "../includes/response.hpp"

std::string	mime_types(std::string &extension)
{
	if ( extension == "html" || extension == "htm" || extension == "shtml" )
		return ("text/html");
	if ( extension == "css" )
		return ("text/css");
	if ( extension == "xml" )
		return ("text/xml");
	if ( extension == "gif" )
		return ("image/gif");
	if ( extension == "jpeg" || extension == "jpg" )
		return ("image/jpeg");
	if ( extension == "js" )
		return ("application/javascript");
	if ( extension == "atom" )
		return ("application/atom+xml");
	if ( extension == "rss" )
		return ("application/rss+xml");
	if ( extension == "mml" )
		return ("text/mathml");
	if ( extension == "txt" )
		return ("text/plain");
	if ( extension == "jad" )
		return ("text/vnd.sun.j2me.app-descriptor");
	if ( extension == "wml" )
		return ("text/vnd.wap.wml");
	if ( extension == "htc" )
		return ("text/x-component");
	if ( extension == "avif" )
		return ("image/avif");
	if ( extension == "png" )
		return ("image/png");
	if ( extension == "svg" || extension == "svgz" )
		return ("image/svg+xml");
	if ( extension == "tif" || extension == "tiff" )
		return ("image/tiff");
	if ( extension == "wbmp" )
		return ("image/vnd.wap.wbmp");
	if ( extension == "webp" )
		return ("image/webp");
	if ( extension == "ico" )
		return ("image/x-icon");
	if ( extension == "jng" )
		return ("image/x-jng");
	if ( extension == "bmp" )
		return ("image/x-ms-bmp");
	if ( extension == "woff" )
		return ("font/woff");
	if ( extension == "woff2" )
		return ("font/woff2");
	if ( extension == "jar" || extension == "war" || extension == "ear" )
		return ("application/java-archive");
	if ( extension == "json" )
		return ("application/json");
	if ( extension == "hqx" )
		return ("application/mac-binhex40");
	if ( extension == "doc" )
		return ("application/msword");
	if ( extension == "pdf" )
		return ("application/pdf");
	if ( extension == "ps" || extension == "eps" || extension == "ai" )
		return ("application/postscript");
	if ( extension == "rtf" )
		return ("application/rtf");
	if ( extension == "m3u8" )
		return ("application/vnd.apple.mpegurl");
	if ( extension == "kml" )
		return ("application/vnd.google-earth.kml+xml");
	if ( extension == "kmz" )
		return ("application/vnd.google-earth.kmz");
	if ( extension == "xls" )
		return ("application/vnd.ms-excel");
	if ( extension == "eot" )
		return ("application/vnd.ms-fontobject");
	if ( extension == "ppt" )
		return ("application/vnd.ms-powerpoint");
	if ( extension == "odg" )
		return ("application/vnd.oasis.opendocument.graphics");
	if ( extension == "odp" )
		return ("application/vnd.oasis.opendocument.presentation");
	if ( extension == "ods" )
		return ("application/vnd.oasis.opendocument.spreadsheet");
	if ( extension == "odt" )
		return ("application/vnd.oasis.opendocument.text");
	if ( extension == "pptx" )
		return ("application/vnd.openxmlformats-officedocument.presentationml.presentation");
	if ( extension == "xlsx" )
		return ("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
	if ( extension == "docx" )
		return ("application/vnd.openxmlformats-officedocument.wordprocessingml.document");
	if ( extension == "wmlc" )
		return ("application/vnd.wap.wmlc");
	if ( extension == "wasm" )
		return ("application/wasm");
	if ( extension == "7z" )
		return ("application/x-7z-compressed");
	if ( extension == "cco" )
		return ("application/x-cocoa");
	if ( extension == "jardiff" )
		return ("application/x-java-archive-diff");
	if ( extension == "jnlp" )
		return ("application/x-java-jnlp-file");
	if ( extension == "run" )
		return ("application/x-makeself");
	if ( extension == "pl" || extension == "pm" )
		return ("application/x-perl");
	if ( extension == "prc" || extension == "pdb" )
		return ("application/x-pilot");
	if ( extension == "rar" )
		return ("application/x-rar-compressed");
	if ( extension == "rpm" )
		return ("application/x-redhat-package-manager");
	if ( extension == "sea" )
		return ("application/x-sea");
	if ( extension == "swf" )
		return ("application/x-shockwave-flash");
	if ( extension == "sit" )
		return ("application/x-stuffit");
	if ( extension == "tcl" || extension == "tk" )
		return ("application/x-tcl");
	if ( extension == "der" || extension == "pem" || extension == "crt" )
		return ("application/x-x509-ca-cert");
	if ( extension == "xpi" )
		return ("application/x-xpinstall");
	if ( extension == "xhtml" )
		return ("application/xhtml+xml");
	if ( extension == "xspf" )
		return ("application/xspf+xml");
	if ( extension == "zip" )
		return ("application/zip");
	if ( extension == "bin" || extension == "exe" || extension == "dll" )
		return ("application/octet-stream");
	if ( extension == "deb" )
		return ("application/octet-stream");
	if ( extension == "dmg" )
		return ("application/octet-stream");
	if ( extension == "iso" || extension == "img" )
		return ("application/octet-stream");
	if ( extension == "msi" || extension == "msp" || extension == "msm" )
		return ("application/octet-stream");
	if ( extension == "mid" || extension == "midi" || extension == "kar" )
		return ("audio/midi");
	if ( extension == "mp3" )
		return ("audio/mpeg");
	if ( extension == "ogg" )
		return ("audio/ogg");
	if ( extension == "m4a" )
		return ("audio/x-m4a");
	if ( extension == "ra" )
		return ("audio/x-realaudio");
	if ( extension == "3gpp" || extension == "3gp" )
		return ("video/3gpp");
	if ( extension == "ts" )
		return ("video/mp2t");
	if ( extension == "mp4" )
		return ("video/mp4");
	if ( extension == "mpeg" || extension == "mpg" )
		return ("video/mpeg");
	if ( extension == "mov" )
		return ("video/quicktime");
	if ( extension == "webm" )
		return ("video/webm");
	if ( extension == "flv" )
		return ("video/x-flv");
	if ( extension == "m4v" )
		return ("video/x-m4v");
	if ( extension == "mng" )
		return ("video/x-mng");
	if ( extension == "asx" || extension == "asf" )
		return ("video/x-ms-asf");
	if ( extension == "wmv" )
		return ("video/x-ms-wmv");
	if ( extension == "avi" )
		return ("video/x-msvideo");
	return ("text/plain");
}
