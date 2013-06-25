#!/bin/bash

WebDir='/afs/cern.ch/cms/CAF/CMSALCA/ALCA_HCALCALIB/HCALMONITORING/RDMweb'
WebSite='http://test-dtlisov.web.cern.ch/test-dtlisov'
HistoDir='/afs/cern.ch/cms/CAF/CMSALCA/ALCA_HCALCALIB/HCALMONITORING/RDMweb/histos'

# print usage info
if [[ "$1" == "" ]]; then
  echo "Usage:"
  echo "  $0 [path]"
  echo "    [path] - place, where RAW files are situated"
  echo "example: ./All_task.sh /afs/cern.ch/work/d/dtlisov/private/Monitoring/data"
  exit
fi

# 1st parameter - path to directory
src=$1

# check, does directory exists
if [[ ! -d $src ]]; then
  echo "ERROR: Directory $src dosn't exists"
  exit 1
fi

# expand path to absolute
fullSrc=$(cd $src; pwd)

echo "Source directory: $fullSrc"

# list all root files:
list=""
totalFiles="0"

for i in $(ls -1 $fullSrc/*.root); do
  let "totalFiles += 1"
  list="$list $i"
done

echo "Total files     : $totalFiles"

# check, do we have output .root files
if [[ "$totalFiles" == "0" ]] ; then
  echo "ERROR: No *.root files found"
  exit 1
fi

# print header to index.html 
echo `cat header.txt`> index_draft.html

prev='194057'
k=0
#download the file with run information
wget http://cmshcalweb01.cern.ch/DetDiag/Local_HTML/runlist.html
cat runlist.html | sed s/\"//g | sed st\>\/t' 'tg | sed sk\<\/td\>kkg | sed sk\<\/a\>kkg | tr '\n' ' ' | awk -F '</tr>' '{for(i=1;i<=NF;i++) printf $i"\n"}' | awk -F '<tr> <td' '{print $2}' | tail -n +4 | sed s/' '/-/g > runlist.tmp
rm runlist.html


#make processing for each file in folder
for i in $list ; do
runnumber=$(echo $i | sed -e 's/[^0-9]*//g')
refnumber=$(echo $prev | sed -e 's/[^0-9]*//g')
let "k = k + 1"
echo
echo
echo
echo 'RUN number ='$runnumber
echo 'RAW file = '`basename $i`
date=''
# extract the date of file
date=$(ls --full-time $fullSrc |grep `basename $i` |awk '{print $6}')

# extract run type, data, time and number of events
type='LED'
for j in `cat runlist.tmp`; do
  RUNNUMBER=$(echo $j | awk -F - '{print $9}')
  if [[ "$runnumber" -eq "$RUNNUMBER" ]] ; then
    type=$(echo $j | awk -F - '{print $13}')
    HTML=$(echo $j | awk -F - '{print $25}' | awk -F 'href=' '{print $2}')
    day=$(echo $j | awk -F - '{print $19}')
    month=$(echo $j | awk -F - '{print $18}')
    year=$(echo $j | awk -F - '{print $17}')
    time=$(echo $j | awk -F - '{print $20}')
    date=$day'.'$month'.'$year 
    wget $HTML
    Nevents=$(cat index.html | tail -n +14 | head -n 1 | awk -F '>' '{print $2}' | awk -F '<' '{print $1}')
    rm index.html
    break
  fi
done
echo 'RUN Date = '$date
echo 'RUN Type = '$type
echo 'Reference RUN number ='$refnumber

#processing
 mkdir $WebDir/LED_$runnumber 
 cmsRun remoteMonitoring_LED_cfg.py $runnumber file:$fullSrc $HistoDir
 root -b -q -l 'RemoteMonitoringMAP.C+("'$HistoDir'/LED_'$runnumber'.root","'$HistoDir'/LED_'$refnumber'.root")'
 mv *.html $WebDir/LED_$runnumber
 mv *.png $WebDir/LED_$runnumber

#adding entry to list of file index_draft.html
let "raw = (k % 2) + 2"
echo '<tr>'>> index_draft.html
echo '<td class="s1" align="center">'$k'</td>'>> index_draft.html
echo '<td class="s'$raw'" align="center">'$runnumber'</td>'>> index_draft.html
echo '<td class="s'$raw'" align="center">'$type'</td>'>> index_draft.html
echo '<td class="s'$raw'" align="center">'$Nevents'</td>'>> index_draft.html
echo '<td class="s'$raw'" align="center">'$date'</td>'>> index_draft.html
echo '<td class="s'$raw'" align="center">'$time'</td>'>> index_draft.html
echo '<td class="s'$raw'" align="center">'$refnumber'</td>'>> index_draft.html
echo '<td class="s'$raw'" align="center"><a href="'$WebSite'/LED_'$runnumber'/MAP.html">LED_'$runnumber'</a></td>'>> index_draft.html
echo '<td class="s'$raw'" align="center"><a href="'$HTML'">DetDiag_'$runnumber'</a></td>'>> index_draft.html
echo '<td class="s'$raw'" align="center">OK</td>'>> index_draft.html
echo '</tr>'>> index_draft.html

prev=$i
done

# print footer to index.html 
echo `cat footer.txt`>> index_draft.html

mv index_draft.html $WebDir/index.html 

#delete temp files
rm runlist.tmp

# check exit code
if [[ $? == 0 ]]; then
  echo "Successfully done!"
else
  rm $dst
  echo "ERROR: Processing failed!"
  exit 1
fi
