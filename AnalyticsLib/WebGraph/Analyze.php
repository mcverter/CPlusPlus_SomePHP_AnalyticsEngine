<?php
//error_reporting(-1);


// CONSTANTS FOR FUNCTIONS
define ("RETURN_VALUE_DELIMITER", "#####");

define ("SECONDS_PER_HOUR", 60*60);
define ("DAYS_PER_WEEK", 7);
define ("MAXIMUM_POSSIBLE_DURATION" , 2 * SECONDS_PER_HOUR);
define ("HOURS_PER_DAY", 24);
define ("SECONDS_PER_DAY", HOURS_PER_DAY * SECONDS_PER_HOUR);


define("EAST_COAST_TIME_ADJUSTMENT", 4 * SECONDS_PER_HOUR);

// Date time formats
define ("FULL_DT_FORMAT", "m/d/Y H:i:s");
define ("UINT_DT_FORMAT", "U");
define("DOW_DT_FORMAT", "l");
define("TOD_DT_FORMAT", "H:00");
define("DATE_DT_FORMAT", "m/d/Y");
define ("DAYHOUR_DT_FORMAT", "m/d H:00");
define("DATETIMEPICKER_DT_FORMAT", "m/d/Y g:i A");
 
// Input Values from HTML form
define("ANALYSIS_TYPE_TOTAL", "total");
define("ANALYSIS_TYPE_AVERAGE", "average");

define ("LAST_TIME_NOW", "now");
define ("LAST_TIME_CHOOSE", "chooseDT");

define ("PERIOD_TYPE_HOURS", "hours");
define ("PERIOD_TYPE_WEEKS", "weeks");

define ("AGG_TYPE_NONE", "none");
define ("AGG_TYPE_TOD", "tod");
define ("AGG_TYPE_DOW",  "dow");


// DETERMINE THE MAXIMUM CUTOFF TIME
function MaxTime($lastTime, $timePeriodType)
{
  $maxTime;

  if ($timePeriodType==PERIOD_TYPE_HOURS)
    {
      $maxTime = DateTime::createFromFormat(UINT_DT_FORMAT,  (floor((date_timestamp_get($lastTime) / SECONDS_PER_HOUR)) *SECONDS_PER_HOUR));
    }
  else if ($timePeriodType==PERIOD_TYPE_WEEKS)
    {
      $maxTime = DateTime::createFromFormat(UINT_DT_FORMAT,  (floor((date_timestamp_get($lastTime) / SECONDS_PER_DAY)) *SECONDS_PER_DAY));
    }
  else 
    {
    }
  return $maxTime;
}


// DETERMINE THE MINIMUM CUTOFF TIME
function  MinTime ($maxTime, $timePeriodType, $timeQty)
{
  if ($timePeriodType==PERIOD_TYPE_HOURS)
    {
      $elapsedTime = "PT" . $timeQty . "H";
    }
  else if ($timePeriodType==PERIOD_TYPE_WEEKS)
    {
      $elapsedTime = "P" . $timeQty . "W";
    }
  else 
    {
    }
  $interval = new DateInterval($elapsedTime);
  $interval->invert=1;
  $minTime = clone $maxTime ;
  $minTime->add ($interval); 
  return $minTime;
}

// YLabel For Graph
function  YLabel($analysisType)
{
  if ($analysisType==ANALYSIS_TYPE_TOTAL)
    {
      return "Total Count";
    }
  else if ($analysisType==ANALYSIS_TYPE_AVERAGE)
    {
      return "Average Duration";
    }
  
}


// XLabel For Graph
function  XLabel($aggregationType, $timePeriodType)
{
  if ($aggregationType==AGG_TYPE_TOD)
    {
      return "Clock Hour";
    }
  else if ($aggregationType==AGG_TYPE_DOW)
    {
      return "Weekday";
    }
  else if ($aggregationType==AGG_TYPE_NONE)
    {
      if ($timePeriodType==PERIOD_TYPE_HOURS)
	{
	  return "Time (hour)";
	}
      else if ($timePeriodType==PERIOD_TYPE_WEEKS)
	{
	  return "Date";
	}
    }
}

// Title For Graph
function  GraphTitle($analysisType, $timePeriodType, $aggregationType, $timeQty, $lastTime) 
{
  $title =  "Foot Traffic" ; //$statisticName;
  if ($analysisType==ANALYSIS_TYPE_TOTAL)
    {
      $title .= ": Total Count ";
    }
  else if ($analysisType==ANALYSIS_TYPE_AVERAGE)
    {
      $title .= ": Average Duration ";
    }


  if ($timePeriodType== PERIOD_TYPE_HOURS)
    {
      $title .= " -- Last " . $timeQty . " Hours";
    }

  else if ($timePeriodType== PERIOD_TYPE_WEEKS)
    {
       $title .= " -- Last " . $timeQty . " Weeks";     
    }
  $title .+ " Since " + $lastTime;

  if ($aggregationType==AGG_TYPE_TOD)
    {
      $title .= " (Aggregated by TOD)";
    }
  else if ($aggregationType == AGG_TYPE_DOW)
    {
      $title .= " (Aggregated by Day) ";
    }
  return $title;
}


// Initializes Arrays for Graph Values  
function InitializeOutputArray($outputArray, $aggregationType, $timePeriodType, $min, $max )
{
  if ($aggregationType==AGG_TYPE_DOW)
    {
      $keys=array("Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday");
      foreach ($keys as $key) {
	  $outputArray[$key]=0;
      }
      return $outputArray;
    }
  else if ($aggregationType==AGG_TYPE_TOD)
    {
      $keys=array("00:00", "01:00", "02:00", "03:00", "04:00", "05:00", "06:00", "07:00", 
		  "08:00", "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", 
		  "16:00", "17:00", "18:00", "19:00", "20:00", "21:00", "22:00", "23:00");
      foreach ($keys as $key) {
	  {$outputArray[$key]=0;}
      }
      return $outputArray;
    }
  else if ($aggregationType==AGG_TYPE_NONE)
    {
      if ($timePeriodType==PERIOD_TYPE_HOURS)
	{
	  for ($time=$min; $time<$max; $time+=SECONDS_PER_HOUR)
	    {
	      $index =  date_format((DateTime::createFromFormat(UINT_DT_FORMAT, $time)), DAYHOUR_DT_FORMAT);
	      $outputArray[$index]=0;
	    }
	  return $outputArray;
	}
      else if ($timePeriodType==PERIOD_TYPE_WEEKS)
	{
	  for ($time=$min; $time<$max; $time+=SECONDS_PER_DAY)
	    {
	      $index =  date_format((DateTime::createFromFormat(UINT_DT_FORMAT, $time)), DATE_DT_FORMAT);
	      $outputArray[$index]=0;
	    }
	  return $outputArray;
	}
      else 
	{
	  die ("Bad Time Period Type");
	}
    }
  else 
    {
      die ("Bad Aggregation Type");
    }
}

// Determines which index to associate with data point
function GraphIndex($aggregationType, $timePeriodType, $start, $minTime )
{
  if ($aggregationType==AGG_TYPE_NONE)
    {
      if ($timePeriodType==PERIOD_TYPE_HOURS)
	{
	  return date_format((DateTime::createFromFormat(UINT_DT_FORMAT, $start)), DAYHOUR_DT_FORMAT);
	}
      elseif ($timePeriodType==PERIOD_TYPE_WEEKS)
	{
	  return date_format((DateTime::createFromFormat(UINT_DT_FORMAT, $start)), DATE_DT_FORMAT);
	}
    }
  elseif ($aggregationType==AGG_TYPE_TOD)
    {
	  return date_format((DateTime::createFromFormat(UINT_DT_FORMAT, $start)), TOD_DT_FORMAT);
    }
  elseif ($aggregationType==AGG_TYPE_DOW)
    {
	  return date_format((DateTime::createFromFormat(UINT_DT_FORMAT, $start)), DOW_DT_FORMAT);
    }
}


// Form values
$filename = $_POST["fileSelector"];
$analysisType=$_POST["statType"];  // => string(5) "total" 
$lastSelector=$_POST["lastselector"]; // string(3) "now" 
$datePicker = $_POST["datepicker"];
$timePicker = $_POST ["timepicker"];
$timePeriodType = $_POST["periodType"]; //) "hours" }


// Hours have "none" as aggregation type
$aggregationType = $_POST["aggType"];
if ($timePeriodType=="hours")
  {
    $aggregationType="none";
    $timeQty = 24;
  }
else if ($timePeriodType=="weeks")
  {
    $timeQty = 4;
  } 

// Get Last Time
$lastTime;
if ($lastSelector==LAST_TIME_NOW) {
  $lastTime = new DateTime("NOW");
}
else 
  {
    $lastTime = DateTime::createFromFormat(DATETIMEPICKER_DT_FORMAT, $datePicker ." " . $timePicker);
  }

// Get Maximum and Minimum Times
$maxTime = MaxTime($lastTime, $timePeriodType);
$minTime = MinTime($maxTime, $timePeriodType, $timeQty);
$max = date_timestamp_get($maxTime);
$min = date_timestamp_get($minTime);

// Initialize Arrays for Data Values
$events = array();
$events = InitializeOutputArray($events, $aggregationType, $timePeriodType, $min, $max);
$durations = array();
$durations = InitializeOutputArray($durations, $aggregationType, $timePeriodType, $min, $max);

// Process CSV file
$filehandle = fopen($filename, "r") or  
  die("Could not open file " . $filename); 


// find index of start and end times
$firstline = rtrim(fgets($filehandle)); // first line is header
$line_fields = explode(",", $firstline);
$startIndex;
$endIndex;

for ($i=0; $i<count($line_fields); $i++)
  {
    if ($line_fields[$i]== "timeStart" ||
	$line_fields[$i]== "entry")
      {
	$startIndex=$i;
      }
    else if ($line_fields[$i]== "timeEnd" ||
	$line_fields[$i]== "exit")
      {
	$endIndex=$i;
      }

  }

// Read each line of CSV file
while (!feof($filehandle))
  {
    $line = rtrim(fgets($filehandle));
    $data_array = explode(",", $line);
    $start_time_str = $data_array[$startIndex];
    $end_time_str =  $data_array[$endIndex];

    // ignore "unknown"s if we are doing Duration
    if (($end_time_str=="unknown") &&
	($analysisType==ANALYSIS_TYPE_AVERAGE))
      {
	continue;
      }

    $start = date_timestamp_get((DateTime::createFromFormat(FULL_DT_FORMAT, $start_time_str))) - EAST_COAST_TIME_ADJUSTMENT;
    $end = date_timestamp_get((DateTime::createFromFormat(FULL_DT_FORMAT, $end_time_str))) - EAST_COAST_TIME_ADJUSTMENT;
    
    // Entries are logged on exit, not on entry
    // Stop analyzing file when entry time > max time + Maximum duration
    if ($start > $max + MAXIMUM_POSSIBLE_DURATION)
      {
	break;
      }
    if (($start >= $min) && ($start <= $max))
      {
	$graphIndex = GraphIndex($aggregationType, $timePeriodType, $start, $min);
	$events[$graphIndex]++;
	$durations[$graphIndex] += ($end-$start);
      } 
  }


$output = array();

// Return totals or Durations 
if ($analysisType == ANALYSIS_TYPE_TOTAL)
  {
    $output = $events;
  }
elseif ($analysisType == ANALYSIS_TYPE_AVERAGE)
  {
    foreach (array_keys($events) as $key)
      {
	if ($events[$key]==0)
	  {
	    $output[$key]=0;
	  }
	else 
	  {
	    $output[$key]=$durations[$key]/$events[$key];
	  }
      }
  }


// Return graph labels and datapoints
$xLabel=XLabel($aggregationType, $timePeriodType);
$yLabel=YLabel($analysisType);
$graphTitle= GraphTitle($analysisType, $timePeriodType, $aggregationType, $timeQty, date_format($lastTime, DATETIMEPICKER_DT_FORMAT)) ;

// return results array
$return_val = print_r($output, true);
$return_val = preg_replace ("/Array/", "", $return_val);
$return_val = preg_replace ("/\(/", "", $return_val);
$return_val = preg_replace ("/\)/", "", $return_val);
$return_val = preg_replace ("/] =>/", ",", $return_val);

$return_val = $xLabel . RETURN_VALUE_DELIMITER . $yLabel . RETURN_VALUE_DELIMITER . $graphTitle . RETURN_VALUE_DELIMITER . $return_val ;

echo $return_val;
?>
