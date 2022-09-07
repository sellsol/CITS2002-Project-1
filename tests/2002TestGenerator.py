import random as rn
import string

processesNum = int(rn.random() * 20) + 1
estimatesNum = int(rn.random() * processesNum) + 1

estimates = []

def randomword(length):
   letters = string.ascii_lowercase
   return ''.join(rn.choice(letters) for i in range(length))

def GenerateEstimatesFile():
   estimatesCount = 0;
   outputString = ""
   while True:
         if (rn.random() > 0.8):
            #Print a comment
            outputString += "# " + str(randomword(int(rn.random() * 20) + 1))
         elif (estimatesCount != estimatesNum):
            estimatesCount += 1
            #Generate an estimate
            name = randomword(int(rn.random() * 40) + 1)
            estimates.append(name)
            outputString += name + " " + str(int(rn.random() * 600) + 1)
         else:
            break
         outputString += "\n"
         
   outFile = open("estimates.txt", "w")
   outFile.write(outputString)
   outFile.close()

def ConvertMonthNumToName(monthNum):
   if (monthNum == 0):
      return "jan"
   elif (monthNum == 1):
      return "feb"
   elif (monthNum == 2):
      return "mar"
   elif (monthNum == 3):
      return "apr"
   elif (monthNum == 4):
      return "may"
   elif (monthNum == 5):
      return "jun"
   elif (monthNum == 6):
      return "jul"
   elif (monthNum == 7):
      return "aug"
   elif (monthNum == 8):
      return "sep"
   elif (monthNum == 9):
      return "oct"
   elif (monthNum == 10):
      return "nov"
   else:
      return "dec"

def GetDaysFromMonth(monthNum):
   if (monthNum == 1):
      return 28
   elif (monthNum == 3 or monthNum == 5 or monthNum == 8 or monthNum == 10):
      return 30
   else:
      return 31

def ConvertDayTypeToName(dayType):
   if (dayType == 0):
      return "sun"
   elif (dayType == 1):
      return "mon"
   elif (dayType == 2):
      return "tue"
   elif (dayType == 3):
      return "wed"
   elif (dayType == 4):
      return "thu"
   elif (dayType == 5):
      return "fri"
   else:
      return "sat"

def GenerateProcessesFile():
   outFile = open("processes.txt", "w")

   processesCount = 0
   errorCount = 1
   while True:
      line = ""
      if (rn.random() > 0.75):
         #Print a comment
         line = "# " + str(randomword(int(rn.random() * 20) + 1))
      elif (processesCount != processesNum):

         processesCount += 1

         #MINUTE
         isAll = True if rn.random() > 0.5 else False
         if (isAll):
            line += "*"
         else:
            line += str(int(rn.random() * 59))

         line += " "

         #HOUR
         isAll = True if rn.random() > 0.5 else False
         if (isAll):
            line += "*"
         else:
            line += str(int(rn.random() * 59))

         line += " "
            
         #MONTH
         month = ""
         numDays = 0
         isAll = True if rn.random() > 0.5 else False
         if (isAll):
            month = "*"
         else:
            monthNum = int(rn.random() * 11)
            numDays = GetDaysFromMonth(monthNum)
            named = True if rn.random() > 0.5 else False
            if (named == True):
               month = ConvertMonthNumToName(monthNum)
            else:
               month = str(monthNum)

         #DAY
         isAll = True if rn.random() > 0.5 else False
         if (isAll):
            line += "*"
         else:
            line += str(int(rn.random() * numDays))

         line += " " + month + " "

         isAll = True if rn.random() > 0.5 else False
         if (isAll):
            line += "*"
         else:
            dayNum = int(rn.random() * 6)
            named = True if rn.random() > 0.5 else False
            if (named == True):
               line += ConvertDayTypeToName(dayNum)
            else:
               line += str(dayNum)

         line += "     " + estimates[int(rn.random() * len(estimates))]
      else:
         break

      outFile.write(line + "\n")

   outFile.close()

GenerateEstimatesFile()
GenerateProcessesFile()
