#include "itkImageFileReader.h"
#include "itkBinaryBorderImageFilter.h"
#include "itkLabelBorderImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkFlatStructuringElement.h"
#include <iomanip>

int main(int, char * argv[])
{
  const int dim = 3;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::BinaryBorderImageFilter< IType, IType > BinaryType;
  BinaryType::Pointer binary = BinaryType::New();
  binary->SetInput( reader->GetOutput() );
  binary->SetForegroundValue( 1 );
  binary->SetFullyConnected( false );
  binary->Update();

  BinaryType::Pointer fbinary = BinaryType::New();
  fbinary->SetInput( reader->GetOutput() );
  fbinary->SetForegroundValue( 1 );
  fbinary->SetFullyConnected( true );
  fbinary->Update();

  typedef itk::LabelBorderImageFilter< IType, IType > LabelType;
  LabelType::Pointer label = LabelType::New();
  label->SetInput( reader->GetOutput() );
  label->SetFullyConnected( false );
  label->Update();

  LabelType::Pointer flabel = LabelType::New();
  flabel->SetInput( reader->GetOutput() );
  flabel->SetFullyConnected( true );
  flabel->Update();
  
  typedef itk::FlatStructuringElement< dim > SRType;
  SRType::RadiusType rad;
  rad.Fill( 1 );
  SRType kernel = SRType::Box( rad );
  
  typedef itk::BinaryErodeImageFilter< IType, IType, SRType > ErodeType;
  ErodeType::Pointer erode = ErodeType::New();
  erode->SetInput( reader->GetOutput() );
  erode->SetKernel( kernel );
  erode->SetForegroundValue( 1 );
  
/*  // write 
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( dilate->GetOutput() );
  writer->SetFileName( argv[2] );*/
  
  reader->Update();
  
  std::cout << "#nb" << "\t" 
            << "b" << "\t" 
            << "fb" << "\t" 
            << "l" << "\t"
            << "fl" << "\t"
            << "e" << std::endl;

  itk::TimeProbe etime;
  for( int t=1; t<=10; t++ )
    {
    itk::TimeProbe btime;
    itk::TimeProbe fbtime;
    itk::TimeProbe ltime;
    itk::TimeProbe fltime;
  
    binary->SetNumberOfThreads( t );
    fbinary->SetNumberOfThreads( t );
    label->SetNumberOfThreads( t );
    flabel->SetNumberOfThreads( t );
    erode->SetNumberOfThreads( t );
    
    for( int i=0; i<10; i++ )
      {
      binary->Modified();
      fbinary->Modified();
      label->Modified();
      flabel->Modified();
      erode->Modified();

      btime.Start();
      binary->Update();
      btime.Stop();
      
      fbtime.Start();
      fbinary->Update();
      fbtime.Stop();
      
      ltime.Start();
      label->Update();
      ltime.Stop();
      
      fltime.Start();
      flabel->Update();
      fltime.Stop();
      
      if( t==1 )
        {
        etime.Start();
        erode->Update();
        etime.Stop();
        }
      }
      
    std::cout << std::setprecision(3) << t << "\t" 
              << btime.GetMeanTime() << "\t"
              << fbtime.GetMeanTime() << "\t"
              << ltime.GetMeanTime() << "\t"
              << fltime.GetMeanTime() << "\t"
              << etime.GetMeanTime() << std::endl;
    }
  
  
  return 0;
}

