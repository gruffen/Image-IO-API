#include "ReadWriteImage.h"

/* Read the image file name */
FILE *fopen( const char * bigImageName, const char * mode);
/* do we need the mode? what really is mode in an image? */

/* Parsing the file*/
char* local_file = bigImageName;

char* ts = strdup(local_file);

char* imageFile = basename(ts); /* return an imageFile.png, .jpeg etc.*/

/* do i need this command above per se? */
/* should i change the char* to str* for it to flow to the code below? */

string findExtension ( const string & imageFile){
	auto position = imageFile.find_last_of('.');

	if (position == string::npos)
		return ""; /* checks to see if it is not found*/

	else {
		string extension( filename.substr( position + 1 ) );
		string fileName( filename.substr(0 ,position - 1) );
		if(regex_search(extension, regex("[^A-Za-z0-9]") )) /* confused on what this regex_search does */
		/* [^A-Za-z0-9] is a space or punctuation - checking for period
		if found, then it is returned as "" */
			return "";
			/* do we not have to subtract extension, regex() from extension in the return value? */
		else
			return extension;



	}
}
string findFileName ( const string & imageFile){
	auto position = imageFile.find_last_of('.');

	if (position == string::npos)
		return ""; /* checks to see if it is not found*/

	else {
		string fileName( filename.substr(0 ,position - 1) );
		if(regex_search(extension, regex("[^A-Za-z0-9]") )) /* confused on what this regex_search does */
		/* [^A-Za-z0-9] is a space or punctuation - checking for period
		if found, then it is returned as "" */
			return "";
			/* do we not have to subtract extension, regex() from extension in the return value? */
		else

			return fileName;


	}
}










int main() {
	vector<string> imageFile; /*{ "picture.jpg" just an example for now, will replace this
	with an array var once i actually know them }*/
	vector<string> extensions(imageFile.size());
	vector<string> fileName(imageFile.size());
	transform( imageFile.begin(), imageFile.end(), extensions.begin(), findExtension);
	transform( imageFile.begin(), imageFile.end(), fileName.begin(), findFileName);

	/* example: imageFile[i] = image.jpeg */
			/* extensions[i] = jpeg */
			  /* fileName[i] = image */



		for( int i = 0; i < imageFile.size(); i++ )
			cout << imageFile[i] << "has extension :" << extensions[i]
		<< "with the name of" << fileName[i]
		/* just to check if the code works */
		/* https://www.rosettacode.org/wiki/Extract_file_extension#C.2B.2B  */


		/* one problem might be that all the vectors still stores non-image type formats like boy.txt
		even someting that doesn't have an extension as it'll store as a null value */

		for (int i = 0; i < imageFile.size(); i++)
			if(extension[i] == "png")
				/* either direct this to the libgpng or just do it here */
			else if (extension[i] == "jpeg")
				/* enter method here */
			else if(extension[i] == "gif")
				/* enter method here */
		 	else if(extension[i] == "tiff")
		 		/* enter method here */
		 	else if (extension[i] == "raw")
		 		/* enter method here */
		 	else
		 		return 1;

		return 0;

}
