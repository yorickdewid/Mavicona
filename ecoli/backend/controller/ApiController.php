<?php

use Ecoli\Response;

class ApiController
{
	/**
	 * Retrieve all packages.
	 *
	 * @return Ecoli\Response
	 */
	public function getMainRepository()
	{
		$object = array(
			'message' => 'Main repository listed',
			'status' => 'OK',
			'success' => true,
		);

		return (new Response($object))->makeJson();
	}

	/**
	 * Add new package.
	 *
	 * @return Ecoli\Response
	 */
	public function doNewPackage()
	{
		$object = array(
			'message' => 'New package added',
			'status' => 'OK',
			'success' => true,
		);

		return (new Response($object))->makeJson();
	}

}