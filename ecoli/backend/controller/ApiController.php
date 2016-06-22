<?php

use Ecoli\Response;
use Ecoli\Database;

class ApiController
{
	/**
	 * Retrieve all packages.
	 *
	 * @return Ecoli\Response
	 */
	public function getMainRepository()
	{
		$db_prs = Database::query('SELECT * FROM package');
		$db_srs = Database::query('SELECT key, value FROM setting WHERE active=true');

		$settings = [];
		foreach ($db_srs as $row) {
			if (is_numeric($row['value'])) {
				$settings[$row['key']] = (int)$row['value'];
			} else {
				$settings[$row['key']] = $row['value'];
			}
		}

		$packages = [];
		foreach ($db_prs as $row) {
			/* Remove PK */
			unset($row['uid']);

			/* Lower name */
			$row['name'] = strtolower($row['name']);

			/* Split database array */
			$keywords = substr($row['keywords'], 1, strlen($row['keywords']) - 2);
			$keyword = explode(",", $keywords);
			$row['keywords'] = $keyword;

			/* Location specific properties */
            $row['location'] = 'http:// ' . $_SERVER['HTTP_HOST'] . '/packages/' . $row['name'] . '/download';
            $row['checksum'] = sha1($row['name']);
            $row['url'] = 'http:// ' . $_SERVER['HTTP_HOST'] . '/packages/' . $row['name'];

			array_push($packages, $row);
		}

		$object = array(
			'meta' => $settings,
			'repositories' => [],
			'packages' => $packages,
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