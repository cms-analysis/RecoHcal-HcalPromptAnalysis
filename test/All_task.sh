#!/bin/bash

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

prev=''
k=0
#download the file with run information
wget http://cmshcalweb01.cern.ch/DetDiag/Local_HTML/runlist.html
cat runlist.html | sed s/\"//g | sed st\>\/t' 'tg | sed sk\<\/td\>kkg | sed sk\<\/a\>kkg | tr '\n' ' ' | awk -F '</tr>' '{for(i=1;i<=NF;i++) printf $i"\n"}' | awk -F '<tr> <td' '{print $2}' | tail -n +4 | sed s/' '/-/g > runlist.tmp



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
 mkdir /afs/cern.ch/user/d/dtlisov/www/RDMweb/LED_$runnumber 
 cmsRun remoteMonitoring_LED_cfg.py $runnumber
 root -b -q -l 'RemoteMonitoringMAP.C+("/afs/cern.ch/work/d/dtlisov/private/Monitoring/histos/LED_'$runnumber'.root","/afs/cern.ch/work/d/dtlisov/private/Monitoring/histos/LED_'$runnumber'.root")'
 mv *.html /afs/cern.ch/user/d/dtlisov/www/RDMweb/LED_$runnumber
 mv *.png /afs/cern.ch/user/d/dtlisov/www/RDMweb/LED_$runnumber

#adding entry to list of file index_draft.html
echo '<tr>'>> index_draft.html
echo '<td class="s4" align="center">'$k'</td>'>> index_draft.html
echo '<td class="s2" align="center">'$runnumber'</td>'>> index_draft.html
echo '<td class="s2" align="center">'$type'</td>'>> index_draft.html
echo '<td class="s2" align="center">'$Nevents'</td>'>> index_draft.html
echo '<td class="s2" align="center">'$date'</td>'>> index_draft.html
echo '<td class="s2" align="center">'$time'</td>'>> index_draft.html
echo '<td class="s2" align="center"><a href="http://test-dtlisov.web.cern.ch/test-dtlisov/LED_196870/MAP.html">LED_'$runnumber'</a></td>'>> index_draft.html
echo '<td class="s2" align="center">OK</td>'>> index_draft.html
echo '</tr>'>> index_draft.html

prev=$i
done

# print footer to index.html 
echo `cat footer.txt`>> index_draft.html

# mv index_draft.html /afs/cern.ch/user/d/dtlisov/www/RDMweb/index.html 

#delete temp files
rm runlist.html
rm runlist.tmp

# check exit code
if [[ $? == 0 ]]; then
  echo "Successfully done!"
else
  rm $dst
  echo "ERROR: Processing failed!"
  exit 1
fi
