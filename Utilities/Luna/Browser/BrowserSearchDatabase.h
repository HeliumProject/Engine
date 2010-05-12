namespace Luna
{

  /// @namespace BrowserSearchDatabase
  /// Namespace is used to contain the browser search information related to updating the database
  namespace BrowserSearchDatabase
  {

    /// @function Initialize
    /// Initializes the Browser search database 
    void Initialize();

    /// @function Cleanup
    /// cleans up the browser search database information
    void Cleanup();

    /// @function UpdateSearchEvents
    /// This is the main function we call to insert data in the to AsssetBrowser search event database
    /// @param The search query object that contains information related to a search that was performed
    /// @return none
    void UpdateSearchEvents( const std::string& query  );
  }
}