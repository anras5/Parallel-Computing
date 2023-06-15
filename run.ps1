# create output/
$OutputFolderPath = "output"
if (-not(Test-Path -Path $OutputFolderPath))
{
    New-Item -ItemType Directory -Path $OutputFolderPath | Out-Null
}

# get csv
$CsvFiles = Get-ChildItem -Path "data" -Filter "*.csv" | Select-Object -ExpandProperty FullName

## build/Debug/sequential
#$SequentialOutputFile = Join-Path -Path $OutputFolderPath -ChildPath "sequential_output.csv"
#foreach ($CsvFile in $CsvFiles)
#{
#    for ($i = 1; $i -le 7; $i++) {
#        & "build/Debug/sequential.exe" $CsvFile | Out-File -FilePath $SequentialOutputFile -Append
#    }
#    Write-Host "build/Debug/sequential | $CsvFile"
#}
#
## build/Debug/openmp
#$OpenMPOutputFile = Join-Path -Path $OutputFolderPath -ChildPath "openmp_output.csv"
#foreach ($CsvFile in $CsvFiles)
#{
#    for ($i = 1; $i -le 7; $i++) {
#        & "build/Debug/openmp.exe" $CsvFile | Out-File -FilePath $OpenMPOutputFile -Append
#    }
#    Write-Host "build/Debug/openmp | $CsvFile"
#}
#
## build/cuda/cuda_program
#$CudaProgramOutputFile = Join-Path -Path $OutputFolderPath -ChildPath "cuda_program_output.csv"
#foreach ($CsvFile in $CsvFiles)
#{
#    for ($i = 1; $i -le 7; $i++) {
#        & "build/cuda/cuda_program.exe" $CsvFile | Out-File -FilePath $CudaProgramOutputFile -Append
#    }
#    Write-Host "build/cuda/cuda_program | $CsvFile"
#}

# build/cuda/cuda_time_kernel
$CudaProgramOutputFile = Join-Path -Path $OutputFolderPath -ChildPath "cuda_time_kernel_output.csv"
foreach ($CsvFile in $CsvFiles)
{
    for ($i = 1; $i -le 7; $i++) {
        & "build/cuda/cuda_time_kernel.exe" $CsvFile | Out-File -FilePath $CudaProgramOutputFile -Append
    }
    Write-Host "build/cuda/cuda_time_kernel | $CsvFile"
}