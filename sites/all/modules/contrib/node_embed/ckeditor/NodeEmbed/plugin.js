CKEDITOR.plugins.add( 'NodeEmbed',
  {
    requires : [ 'iframedialog' ],
    
    //Initialize the plugin
    init : function( editor )
    {
      //add the popup for the plugin
      CKEDITOR.dialog.add( 'nodeembed', function ()
        {
          
          return {
            
            title : 'Node Embed', 
            minWidth : 700,
            minHeight : 400,
            //the contents of the dialog
            contents : 
              [
                {
                  id : 'iframe',
                  label : 'Embed a Node: ',
                  expand : true,
                  elements :
                    [
                      {
                        type : 'iframe',
                        src : Drupal.settings.basePath + 'ckeditor-node-embed',
                        width : '100%',
                        height : '100%',
                        onContentLoad : function() {}
                      }
                    ]
                }
              ],
              
            //handles the 'ok' button being clicked  
            onOk : function() {
              var node_id = window.currentActiveNid; //set or updated whenever a node is selected

              if( node_id != null && node_id != "" ) {
                this._.editor.insertHtml('<div class="embed">[[nid:' + node_id + ']]</div>');  //insert the tag into the editor
              }
            }  
          }; //end onOK   		
        
        } //end return
         
      ); //end addDialog

      //add the button to the menu
      editor.ui.addButton( 'NodeEmbed',
	      {
		      label : 'Node Embed',
		      icon : this.path + 'images/icon.gif',
		      command : 'NodeEmbed'
	      } 
	    );

      //add the command to open the window
      editor.addCommand( 'NodeEmbed', {exec : function(e){
		        e.openDialog('nodeembed');
	        }
        }
      );

      //if the "menu" plugin is loaded, register the menu items.
      if ( editor.addMenuItems )
      {
	      editor.addMenuItems(
		      {
			      nodemenu :
			      {
				      label : 'Node Embed',
				      command : 'NodeEmbed',
				      group : 'NodeEmbed',
				      order : 1
			      }
		      }
		    );
      } //end if
    
    }//end init
    
  } ); //end add
