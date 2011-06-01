#ifndef __Panzer_STKConnManager_hpp__
#define __Panzer_STKConnManager_hpp__

#include <vector>

// Teuchos includes
#include "Teuchos_RCP.hpp"

// Panzer includes
#include "Panzer_ConnManager.hpp"

#include "Panzer_STK_Interface.hpp"
#include "Panzer_IntrepidFieldPattern.hpp"

namespace panzer_stk {

class STKConnManager : public panzer::ConnManager<int,int> {
public:
   STKConnManager(const Teuchos::RCP<STK_Interface> & stkMeshDB);

   virtual ~STKConnManager() {}

   /** Tell the connection manager to build the connectivity assuming
     * a particular field pattern.
     *
     * \param[in] fp Field pattern to build connectivity for
     */
   virtual void buildConnectivity(const panzer::FieldPattern & fp);

   /** Get ID connectivity for a particular element
     *
     * \param[in] localElmtId Local element ID
     *
     * \returns Pointer to beginning of indices, with total size
     *          equal to <code>getConnectivitySize(localElmtId)</code>
     */
   virtual const GlobalOrdinal * getConnectivity(LocalOrdinal localElmtId) const 
   { return &connectivity_[elmtLidToConn_[localElmtId]]; }

   /** Get ID connectivity for a particular element
     *
     * \param[in] localElmtId Local element ID
     *
     * \returns Pointer to beginning of indices, with total size
     *          equal to <code>getConnectivitySize(localElmtId)</code>
     */
   virtual GlobalOrdinal * getConnectivity(LocalOrdinal localElmtId) 
   { return &connectivity_[elmtLidToConn_[localElmtId]]; }

   /** How many mesh IDs are associated with this element?
     *
     * \param[in] localElmtId Local element ID
     *
     * \returns Number of mesh IDs that are associated with this element.
     */
   virtual LocalOrdinal getConnectivitySize(LocalOrdinal localElmtId) const
   { return connSize_[localElmtId]; }

   /** Get the block ID for a particular element.
     *
     * \param[in] localElmtId Local element ID
     */
   virtual std::string getBlockId(LocalOrdinal localElmtId) const;

   /** How many element blocks in this mesh?
     */
   virtual std::size_t numElementBlocks() const
   { return stkMeshDB_->getNumElementBlocks(); }

   /** Get block IDs from STK mesh object
     */
   virtual void getElementBlockIds(std::vector<std::string> & elementBlockIds) const
   { return stkMeshDB_->getElementBlockNames(elementBlockIds); }

   /** Get the local element IDs for a paricular element
     * block.
     *
     * \param[in] blockIndex Block Index
     *
     * \returns Vector of local element IDs.
     */
   virtual const std::vector<LocalOrdinal> & getElementBlock(const std::string & blockId) const
   { return *(elementBlocks_.find(blockId)->second); }

   /** Get the coordinates (with local cell ids) for a specified element block and field pattern.
     * 
     * \param[in] blockId Block containing the cells
     * \param[in] coordProvider Field pattern that builds the coordinates
     * \param[out] localCellIds Local cell Ids (indices)
     * \param[out] Resizable field container that contains the coordinates 
     *             of the points on exit.
     */
   virtual void getDofCoords(const std::string & blockId,
                             const panzer::IntrepidFieldPattern & coordProvider,
                             std::vector<std::size_t> & localCellIds,
                             Intrepid::FieldContainer<double> & points) const;

    /** Get STK interface that this connection manager is built on.
      */
    Teuchos::RCP<STK_Interface> getSTKInterface() const
    { return stkMeshDB_; }

protected:
   /** Apply periodic boundary conditions associated with the mesh object.
     *
     * \note This function requires global All-2-All communication IFF
     *       periodic boundary conditions are required.
     */
   void applyPeriodicBCs( const panzer::FieldPattern & fp, GlobalOrdinal nodeOffset, GlobalOrdinal edgeOffset, 
                                                           GlobalOrdinal faceOffset, GlobalOrdinal cellOffset);

   void buildLocalElementMapping();
   void clearLocalElementMapping();
   void buildOffsetsAndIdCounts(const panzer::FieldPattern & fp,
                                LocalOrdinal & nodeIdCnt, LocalOrdinal & edgeIdCnt,
                                LocalOrdinal & faceIdCnt, LocalOrdinal & cellIdCnt,
                                GlobalOrdinal & nodeOffset, GlobalOrdinal & edgeOffset,
                                GlobalOrdinal & faceOffset, GlobalOrdinal & cellOffset) const;

   LocalOrdinal addSubcellConnectivities(stk::mesh::Entity * element,unsigned subcellRank,
                                         LocalOrdinal idCnt,GlobalOrdinal offset);

   void modifySubcellConnectivities(const panzer::FieldPattern & fp, stk::mesh::Entity * element,
                                    unsigned subcellRank,unsigned subcellId,GlobalOrdinal newId,GlobalOrdinal offset);

   Teuchos::RCP<STK_Interface> stkMeshDB_;

   Teuchos::RCP<std::vector<stk::mesh::Entity*> > elements_;

   // element block information
   std::map<std::string,Teuchos::RCP<std::vector<LocalOrdinal> > > elementBlocks_;
   std::map<std::string,GlobalOrdinal> blockIdToIndex_;

   std::vector<LocalOrdinal> elmtLidToConn_; // element LID to Connectivity map
   std::vector<LocalOrdinal> connSize_; // element LID to Connectivity map
   std::vector<GlobalOrdinal> connectivity_; // Connectivity
};

}

#endif
