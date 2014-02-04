INSTALL LAMP SERVER ON UBUNTU
-----------------------------
sudo apt-get install tasksel
sudo tasksel
Choose "LAMP Server"
Press "Okay"


USING WebGraph on LOCALHOST
---------------------------

cd <YOUR_DIRECTORY>
git pull https://CuriousMitchell@bitbucket.org/billy_curiomotion/curio_mitchell.git

WebGraph will be in 
YOUR_DIRECTORY/curio_mitchell/AnalyticsLib/WebGraph

Apache Root Directory is typically /var/www

cd /var/www

sudo ln -s YOUR_DIRECTORY/curio_mitchell/AnalyticsLib/WebGraph .

Start Apache
apachectl start

WebGraph will be available at:
http://localhost/WebGraph/AnalyticsWebGui.html


IMPORTANT FILES:
----------------

AnalyticsWebGui.html : Contains all the javascript and html for the web display
Analyze.php:  Used by the AJAX Form Submission.  Processes the CSV files to creates data for the graph.


javascript/ : Local directory for all javascript files
css/ : Local directory for all css files.
